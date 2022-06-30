package com.meishe.msopencv;

import android.view.Surface;

/**
 * @author : lpf
 * @FileName: MSFaceLocation
 * @Date: 2022/6/30 23:50
 * @Description: 人脸定位
 */
public class MSFaceLocation {
    /**
     * c++层对象指针
     */
    private long mNativeObj = 0;

    /**
     * @param model 人脸定位模型
     */
    public MSFaceLocation(String model) {
        mNativeObj = nativeCreateObject(model);
    }


    /**
     * 设置显示的画布
     * @param surface
     */
    public void setSurface(Surface surface) {
        nativeSetSurface(mNativeObj, surface);
    }

    /**
     * 开启人脸跟踪
     */
    public void start() {
        nativeStart(mNativeObj);
    }

    /**
     * 关闭
     */
    public void stop() {
        nativeStop(mNativeObj);
    }

    /**
     * 人脸定位 同时显示在surface中
     * @param inputImage  图像数据
     * @param width
     * @param height
     * @param rotationDegrees
     */
    public void detect(byte[] inputImage, int width, int height, int rotationDegrees) {
        nativeDetect(mNativeObj, inputImage, width, height, rotationDegrees);
    }

    /**
     * 释放
     */
    public void release() {
        nativeRelease(mNativeObj);
        mNativeObj = 0;
    }




    /*-----------------------------native-------------------------------*/
    private native long nativeCreateObject(String model);
    private native void nativeSetSurface(long mNativeObj, Surface surface);
    private static native void nativeRelease(long thiz);
    private static native void nativeStart(long thiz);
    private static native void nativeStop(long thiz);
    private static native void nativeDetect(long thiz, byte[] inputImage, int width, int height, int rotationDegrees);

}
