package com.meishe.msopencv;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;

import com.meishe.msopencv.databinding.ActivityPicReduceBinding;

public class PicReduceActivity extends AppCompatActivity {

    private ActivityPicReduceBinding mBinding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mBinding=ActivityPicReduceBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        Bitmap bitmap = BitmapFactory.decodeFile("/sdcard/pic.png", new BitmapFactory.Options());
        mBinding.ivOld.setImageBitmap(bitmap);


        mBinding.btnGray.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mBinding.ivNew.setImageBitmap(ImageProcess.getGrayBitmap("/sdcard/pic.png", Bitmap.Config.ARGB_8888));
            }
        });

        mBinding.btnTwo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mBinding.ivNew.setImageBitmap(ImageProcess.getTwoBitmap("/sdcard/pic.png", Bitmap.Config.ARGB_8888));
            }
        });


    }
}