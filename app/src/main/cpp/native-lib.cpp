#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
/*输出日志*/
#include <android/log.h>

#define LOG_TAG "native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


#define DEFAULT_CARD_WIDTH 640
#define DEFAULT_CARD_HEIGHT 400
#define  FIX_IDCARD_SIZE Size(DEFAULT_CARD_WIDTH,DEFAULT_CARD_HEIGHT)
#define FIX_TEMPLATE_SIZE  Size(153, 28)

extern "C" {

using namespace cv;
using namespace std;
/**
 * 工具方法 bitmap 转 mat2
 * @param env
 * @param bitmap
 * @param m_addr
 * @param needUnPremultiplyAlpha
 */
extern JNIEXPORT void JNICALL Java_org_opencv_android_Utils_nBitmapToMat2
        (JNIEnv *env, jclass, jobject bitmap, jlong m_addr, jboolean needUnPremultiplyAlpha);
/**
 * 工具方法 Mat 转bitmap
 * @param env
 * @param m_addr
 * @param bitmap
 */
extern JNIEXPORT void JNICALL Java_org_opencv_android_Utils_nMatToBitmap
        (JNIEnv *env, jclass, jlong m_addr, jobject bitmap);

/**
 * 将Mat 数据转给Bitmap 返回bitmap对象
 * @param env
 * @param srcData
 * @param config
 * @return
 */
jobject createBitmap(JNIEnv *env, Mat srcData, jobject config) {
    /*图片信息*/
    int imgWidth = srcData.cols;
    int imgHeight = srcData.rows;
    int numPix = imgWidth * imgHeight;

    /*反射拿到Bitmap*/
    jclass bmpCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(bmpCls, "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    /*执行静态方法 得到Bitmap对象*/
    jobject jBmpObj = env->CallStaticObjectMethod(bmpCls, createBitmapMid, imgWidth, imgHeight,
                                                  config);
    /*将mat 数据传给Bitmap*/
    Java_org_opencv_android_Utils_nMatToBitmap(env, 0, (jlong) &srcData, jBmpObj);
    return jBmpObj;
}


}



extern "C" JNIEXPORT jstring JNICALL
Java_com_meishe_msopencv_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
/**
 * 获取图片身份证号核心区域
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_meishe_msopencv_ImageProcess_getIdNumber(JNIEnv *env, jclass clazz, jobject src,
                                                  jobject config) {
    Mat src_img;
    Mat dst_img;
    //imshow("src_", src_img);
    /*将bitmap转换为Mat型格式数据*/
    Java_org_opencv_android_Utils_nBitmapToMat2(env, clazz, src, (jlong) &src_img, 0);

    Mat dst;
    /*无损压缩 640*400*/
    resize(src_img, src_img,FIX_IDCARD_SIZE);
    //imshow("dst", src_img);
    /*灰度化*/
    cvtColor(src_img, dst, COLOR_BGR2GRAY);
    //imshow("gray", dst);

    /*二值化*/
    threshold(dst, dst, 150, 255, CV_THRESH_BINARY);
    //imshow("threshold", dst);

    /*膨胀*/
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(20, 10));
    erode(dst, dst, erodeElement);
    //imshow("erode", dst);

    /*轮廓检测 arraylist*/
    vector< vector<Point> > contours;
    vector<Rect> rects;

    findContours(dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect(contours.at(i));
        //rectangle(dst, rect, Scalar(0, 0, 255));  // 在dst 图片上显示 rect 矩形
        if (rect.width > rect.height * 9) {
            rects.push_back(rect);
            rectangle(dst, rect, Scalar(0,255,255));
            dst_img = src_img(rect);
        }

    }
    // imshow("轮廓检测", dst);


    if (rects.size() == 1) {
        Rect rect = rects.at(0);
        dst_img = src_img(rect);
    }else {
        int lowPoint = 0;
        Rect finalRect;
        for (int i = 0; i < rects.size(); i++)
        {
            Rect rect = rects.at(i);
            Point p = rect.tl();
            if (rect.tl().y > lowPoint) {
                lowPoint = rect.tl().y;
                finalRect = rect;
            }
        }
        rectangle(dst, finalRect, Scalar(255, 255, 0));
        //imshow("contours", dst);
        dst_img = src_img(finalRect);
    }

    /*身份证核心区域生成bitmap*/
    jobject  bitmap = createBitmap(env,dst_img,config);


    src_img.release();
    dst_img.release();
    dst.release();

    return  bitmap;

}

/**
 * 返回灰度图
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_meishe_msopencv_ImageProcess_getGrayBitmap(JNIEnv *env, jclass clazz, jstring pic_path, jobject config) {
     const char* path=env->GetStringUTFChars(pic_path,JNI_OK);

     Mat src= imread(path);
     Mat des;
     cvtColor(src,des,COLOR_BGR2GRAY);

    int imgWidth = des.cols;
    int imgHeight = des.rows;

    /*反射拿到Bitmap*/
    jclass bmpCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(bmpCls, "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    /*执行静态方法 得到Bitmap对象*/
    jobject jBmpObj = env->CallStaticObjectMethod(bmpCls, createBitmapMid, imgWidth, imgHeight,
                                                  config);
    /*将mat 数据传给Bitmap*/
    Java_org_opencv_android_Utils_nMatToBitmap(env, 0, (jlong) &des, jBmpObj);

    env->ReleaseStringUTFChars(pic_path,path);
    return jBmpObj;

}
/**
 * 二值化
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_meishe_msopencv_ImageProcess_getTwoBitmap(JNIEnv *env, jclass clazz, jstring pic_path,
                                                   jobject config) {
    const char* path=env->GetStringUTFChars(pic_path,JNI_OK);
    Mat src= imread(path);
    Mat des;
    cvtColor(src,des,COLOR_BGR2GRAY);

    int imgWidth = des.cols;
    int imgHeight = des.rows;

    threshold(des,des,150,250,CV_THRESH_BINARY);

    /*反射拿到Bitmap*/
    jclass bmpCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(bmpCls, "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    /*执行静态方法 得到Bitmap对象*/
    jobject jBmpObj = env->CallStaticObjectMethod(bmpCls, createBitmapMid, imgWidth, imgHeight,
                                                  config);
    /*将mat 数据传给Bitmap*/
    Java_org_opencv_android_Utils_nMatToBitmap(env, 0, (jlong) &des, jBmpObj);

    env->ReleaseStringUTFChars(pic_path,path);
    return jBmpObj;

}