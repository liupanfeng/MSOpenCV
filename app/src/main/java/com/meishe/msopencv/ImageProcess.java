package com.meishe.msopencv;

import android.graphics.Bitmap;

/**
 * @author : lpf
 * @FileName: ImageProcess
 * @Date: 2022/6/19 0:16
 * @Description:
 */
public class ImageProcess {

    static {
        System.loadLibrary("msopencv");
    }

    public static native Bitmap getIdNumber(Bitmap src, Bitmap.Config config);
}
