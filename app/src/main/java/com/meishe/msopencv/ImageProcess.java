package com.meishe.msopencv;

import android.graphics.Bitmap;

/**
 * @author : lpf
 * @FileName: ImageProcess
 * @Date: 2022/6/19 0:16
 * @Description:
 */
public class ImageProcess {


    /**
     * 获取图片号码 核心区域
     * @param src
     * @param config
     * @return
     */
    public static native Bitmap getIdNumber(Bitmap src, Bitmap.Config config);

    /**
     * 灰度化
     * @param picPath
     * @param config
     * @return
     */
    public static native Bitmap getGrayBitmap(String picPath,Bitmap.Config config);

    /**
     * 二值化
     * @param picPath
     * @param config
     * @return
     */
    public static native Bitmap getTwoBitmap(String picPath,Bitmap.Config config);

}
