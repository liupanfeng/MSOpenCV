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

    /**
     * 获取图片号码 核心区域
     * @param src
     * @param config
     * @return
     */
    public static native Bitmap getIdNumber(Bitmap src, Bitmap.Config config);
}
