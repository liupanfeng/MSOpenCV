package com.meishe.msopencv.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.MediaStore;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.googlecode.tesseract.android.TessBaseAPI;
import com.meishe.msopencv.ImageProcess;
import com.meishe.msopencv.R;
import com.meishe.msopencv.utils.PathUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import io.reactivex.Observable;
import io.reactivex.disposables.Disposable;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;

public class MSIDCardIdentificationActivity extends AppCompatActivity {

    private final static String TAG = MSIDCardIdentificationActivity.class.getSimpleName();
    private TessBaseAPI mTessBaseAPI;
    private String mLanguage = "cn";
    private TextView mTvCardNumberView;
    private ImageView mIvCardView;
    private Bitmap mResultImage;
    private Bitmap mFullImage;
    private Disposable mSubscribe;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_msidcard_identification);

        mIvCardView = (ImageView) findViewById(R.id.iv_card_view);
        mTvCardNumberView = (TextView) findViewById(R.id.tv_card_number_view);
        initTess();
    }


    private void initTess() {
         mSubscribe = Observable.just(1).observeOn(Schedulers.io()).subscribe(new Consumer<Integer>() {
            @Override
            public void accept(Integer integer) throws Exception {
                Log.e("lpf","----doInBackground---");
                mTessBaseAPI = new TessBaseAPI();
                try {
                    InputStream is = null;
                    is = getAssets().open(mLanguage + ".traineddata");
                    File file = new File(PathUtils.getModelDir()+File.separator + mLanguage + ".traineddata");
                    if (!file.exists()) {
                        file.getParentFile().mkdirs();
                        FileOutputStream fos = new FileOutputStream(file);
                        byte[] buffer = new byte[2048];
                        int len;
                        while ((len = is.read(buffer)) != -1) {
                            fos.write(buffer, 0, len);
                        }
                        fos.close();
                    }
                    is.close();
                    PathUtils.getModelDir();
                    mTessBaseAPI.init(PathUtils.getRootDir(), mLanguage);
                } catch (IOException e) {
                    e.printStackTrace();
                    Log.e("lpf","----copy error:"+e.getMessage());
                }
            }
        });
    }

    /**
     * 搜索
     * @param view
     */
    public void search(View view) {
        Intent intent;
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
            intent = new Intent();
            intent.setAction(Intent.ACTION_GET_CONTENT);
        } else {
            intent = new Intent(Intent.ACTION_PICK,
                    MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        }
        intent.setType("image/*");
        //使用选取器并自定义标题
        startActivityForResult(Intent.createChooser(intent, "选择待识别图片"), 100);
    }


    public void searchId(View view) {
        mTvCardNumberView.setText(null);
        mResultImage = null;
        Bitmap bitmapResult = ImageProcess.getIdNumber(mFullImage, Bitmap.Config.ARGB_8888);
        mFullImage.recycle();
        mResultImage = bitmapResult;
        //tesseract-ocr
        mIvCardView.setImageBitmap(bitmapResult);
    }

    public void recognition(View view) {
        // 识别Bitmap中的图片
        mTessBaseAPI.setImage(mResultImage);
        mTvCardNumberView.setText(mTessBaseAPI.getUTF8Text());
        mTessBaseAPI.clear();
    }

    private void getResult(Uri uri) {
//        safeRecycled();
        String imagePath = null;
        if (null != uri) {
            //在我们的魅族测试手机上发现有一个相册管家 从这里选取图片会得到类似
            //file:///storage/emulated/0/tencent/MicroMsg/WeiXin/mmexport1474966179606.jpg的uri
            if ("file".equals(uri.getScheme())) {
                Log.i(TAG, "path uri 获得图片");
                imagePath = uri.getPath();
            } else if ("content".equals(uri.getScheme())) {
                Log.i(TAG, "content uri 获得图片");
                String[] filePathColumns = {MediaStore.Images.Media.DATA};
                Cursor c = getContentResolver().query(uri, filePathColumns, null, null, null);
                if (null != c) {
                    if (c.moveToFirst()) {
                        int columnIndex = c.getColumnIndex(filePathColumns[0]);
                        imagePath = c.getString(columnIndex);
                    }
                    c.close();
                }
            }
        }
        if (!TextUtils.isEmpty(imagePath)) {
            if (mFullImage != null) {
                mFullImage.recycle();
            }
            mFullImage = toBitmap(imagePath);
            mTvCardNumberView.setText(null);
            mIvCardView.setImageBitmap(mFullImage);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 100 && null != data) {
            getResult(data.getData());
        }
    }

    public static Bitmap toBitmap(String pathName) {
        if (TextUtils.isEmpty(pathName)){
            return null;
        }
        BitmapFactory.Options o = new BitmapFactory.Options();
        o.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(pathName, o);
        int width_tmp = o.outWidth, height_tmp = o.outHeight;
        int scale = 1;
        while (true) {
            if (width_tmp <= 640 && height_tmp <= 480) {
                break;
            }
            width_tmp /= 2;
            height_tmp /= 2;
            scale *= 2;
        }
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inSampleSize = scale;
        opts.outHeight = height_tmp;
        opts.outWidth = width_tmp;
        return BitmapFactory.decodeFile(pathName, opts);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mTessBaseAPI !=null){
            mTessBaseAPI.end();
        }
        if (mSubscribe!=null){
            mSubscribe.dispose();
        }
    }
}