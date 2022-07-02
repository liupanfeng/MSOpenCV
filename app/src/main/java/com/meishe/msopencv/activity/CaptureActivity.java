package com.meishe.msopencv.activity;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.ImageCapture;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.core.content.ContextCompat;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.util.Size;
import android.view.SurfaceHolder;

import com.google.common.util.concurrent.ListenableFuture;
import com.meishe.msopencv.MSFaceLocation;
import com.meishe.msopencv.databinding.ActivityCaptureBinding;
import com.meishe.msopencv.utils.Utils;

import java.util.concurrent.ExecutionException;


/**
 * 人像追踪页面
 */
public class CaptureActivity extends AppCompatActivity
        implements SurfaceHolder.Callback, ImageAnalysis.Analyzer {

    private static final String mCascadeFileName="lbpcascade_frontalface.xml";
    private ActivityCaptureBinding mBinding;
    private MSFaceLocation mMSFaceLocation;

    private ListenableFuture<ProcessCameraProvider> cameraProviderFuture;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mBinding = ActivityCaptureBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        initCamera();
        String path = Utils.copyAsset2Dir(this, mCascadeFileName);
        mMSFaceLocation = new MSFaceLocation(path);
        mMSFaceLocation.start();
        mBinding.surfaceView.getHolder().addCallback(this);
    }


    private void initCamera() {
        ListenableFuture<ProcessCameraProvider> cameraProviderFuture =
                ProcessCameraProvider.getInstance(this);
        Log.d("lpf","processCameraProvider=");
        cameraProviderFuture.addListener(new Runnable() {
            @Override
            public void run() {
                try {
                    Log.d("lpf","run-----------------");
                    ProcessCameraProvider cameraProvider = cameraProviderFuture.get();
                    bindAnalysis(cameraProvider);
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        }, ContextCompat.getMainExecutor(this));
    }



    /**
     * 图片分析：得到摄像头图像数据
     * STRATEGY_KEEP_ONLY_LATEST :非阻塞模式，每次获得最新帧
     * STRATEGY_BLOCK_PRODUCER : 阻塞模式，处理不及时会导致降低帧率
     *
     * @param processCameraProvider
     */
    private void bindAnalysis(ProcessCameraProvider processCameraProvider) {
        ImageAnalysis imageAnalysis = new ImageAnalysis.Builder()
                .setTargetResolution(new Size(640, 480))
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build();
        imageAnalysis.setAnalyzer(ContextCompat.getMainExecutor(this), this);

        processCameraProvider.unbindAll();
        Log.d("lpf","bindToLifecycle-----------------");
        processCameraProvider.bindToLifecycle(this,
                CameraSelector.DEFAULT_FRONT_CAMERA, imageAnalysis);
    }



    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.d("lpf","surfaceCreated-------");
        mMSFaceLocation.setSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.d("lpf","surfaceChanged-------");
//        mMSFaceLocation.setSurface(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.d("lpf","surfaceDestroyed-------");
//        mMSFaceLocation.setSurface(null);
    }

    /**
     * 摄像头数据回调
     *
     * @param image
     */
    @Override
    public void analyze(@NonNull ImageProxy image) {
        Log.d("lpf","image.getWidth="+image.getWidth()
                +" image.getHeight="+image.getHeight());
        byte[] buffer = Utils.getDataFromImage(image);
        mMSFaceLocation.detect(buffer, image.getWidth(), image.getHeight(),
                image.getImageInfo().getRotationDegrees());
        image.close();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMSFaceLocation.release();
    }
}