#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
/*输出日志*/
#include "MSFaceLocation.h"
#include <android/log.h>

#define LOG_TAG "lpf"
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
    resize(src_img, src_img, FIX_IDCARD_SIZE);
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
    vector<vector<Point> > contours;
    vector<Rect> rects;

    findContours(dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    for (int i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours.at(i));
        //rectangle(dst, rect, Scalar(0, 0, 255));  // 在dst 图片上显示 rect 矩形
        if (rect.width > rect.height * 9) {
            rects.push_back(rect);
            rectangle(dst, rect, Scalar(0, 255, 255));
            dst_img = src_img(rect);
        }

    }
    // imshow("轮廓检测", dst);


    if (rects.size() == 1) {
        Rect rect = rects.at(0);
        dst_img = src_img(rect);
    } else {
        int lowPoint = 0;
        Rect finalRect;
        for (int i = 0; i < rects.size(); i++) {
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
    jobject bitmap = createBitmap(env, dst_img, config);


    src_img.release();
    dst_img.release();
    dst.release();

    return bitmap;

}

/**
 * 返回灰度图
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_meishe_msopencv_ImageProcess_getGrayBitmap(JNIEnv *env, jclass clazz, jstring pic_path,
                                                    jobject config) {
    const char *path = env->GetStringUTFChars(pic_path, JNI_OK);

    Mat src = imread(path);
    Mat des;
    cvtColor(src, des, COLOR_BGR2GRAY);

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

    env->ReleaseStringUTFChars(pic_path, path);
    return jBmpObj;

}
/**
 * 二值化
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_meishe_msopencv_ImageProcess_getTwoBitmap(JNIEnv *env, jclass clazz, jstring pic_path,
                                                   jobject config) {
    const char *path = env->GetStringUTFChars(pic_path, JNI_OK);
    Mat src = imread(path);
    Mat des;
    cvtColor(src, des, COLOR_BGR2GRAY);

    int imgWidth = des.cols;
    int imgHeight = des.rows;

    threshold(des, des, 150, 250, CV_THRESH_BINARY);

    /*反射拿到Bitmap*/
    jclass bmpCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(bmpCls, "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    /*执行静态方法 得到Bitmap对象*/
    jobject jBmpObj = env->CallStaticObjectMethod(bmpCls, createBitmapMid, imgWidth, imgHeight,
                                                  config);
    /*将mat 数据传给Bitmap*/
    Java_org_opencv_android_Utils_nMatToBitmap(env, 0, (jlong) &des, jBmpObj);

    env->ReleaseStringUTFChars(pic_path, path);
    return jBmpObj;

}


//extern "C"
//JNIEXPORT jlong JNICALL
//Java_com_meishe_msopencv_MSFaceLocation_nativeCreateObject(JNIEnv *env, jobject thiz,
//                                                           jstring model) {
//    const char *_model = env->GetStringUTFChars(model, JNI_OK);
//    MSFaceLocation *msFaceLocation = new MSFaceLocation(_model);
//    env->ReleaseStringUTFChars(model, _model);
//    LOGI("nativeCreateObject success");
//    return reinterpret_cast<jlong>(msFaceLocation);
//
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_meishe_msopencv_MSFaceLocation_nativeSetSurface(JNIEnv *env, jobject thiz,
//                                                         jlong m_native_obj, jobject surface) {
//    if (thiz != 0) {
//        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
//        if (!surface) {
//            msFaceLocation->setANativeWindow(nullptr);
//            return;
//        }
//
//        msFaceLocation->setANativeWindow(ANativeWindow_fromSurface(env, surface));
//    }
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_meishe_msopencv_MSFaceLocation_nativeRelease(JNIEnv *env, jclass clazz, jlong thiz) {
//    if (thiz != 0) {
//        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
//        msFaceLocation->tracker->stop();
//        delete msFaceLocation;
//    }
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_meishe_msopencv_MSFaceLocation_nativeStart(JNIEnv *env, jclass clazz, jlong thiz) {
//    if (thiz != 0) {
//        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
//        msFaceLocation->tracker->run();
//    }
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_meishe_msopencv_MSFaceLocation_nativeStop(JNIEnv *env, jclass clazz, jlong thiz) {
//    if (thiz != 0) {
//        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
//        msFaceLocation->tracker->stop();
//    }
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_meishe_msopencv_MSFaceLocation_nativeDetect(JNIEnv *env, jclass clazz, jlong thiz,
//                                                     jbyteArray input_image, jint width,
//                                                     jint height, jint rotation_degrees) {
//    if (thiz == 0) {
//        return;
//    }
//
//    MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
//    jbyte *_input_image = env->GetByteArrayElements(input_image, JNI_OK);
//    LOGI("with=%d,height=%d",width,height);
//    /*cameraX 传递过来的数据是i420*/
//    Mat src(height * 3 / 2, width, CV_8UC1, _input_image);
//    /*yuv420 转 rgba i420*/
//    cvtColor(src, src, CV_YUV2RGBA_I420);
//    if (rotation_degrees == 90) {
//        /*正向*/
//        rotate(src, src, ROTATE_90_CLOCKWISE);
//    } else if (rotation_degrees == 270) {
//        /*反向*/
//        rotate(src, src, ROTATE_90_COUNTERCLOCKWISE);
//    }
//    /*镜像选装*/
////    flip(src,src,1);
//    Mat gray;
//    cvtColor(src, gray, CV_BGR2GRAY);
//    equalizeHist(gray, gray);
//    msFaceLocation->tracker->process(gray);
//    std::vector<Rect> faces;
//    msFaceLocation->tracker->getObjects(faces);
//    for (Rect face:faces) {
//        rectangle(src, face, Scalar(255, 0, 0));
//    }
//    msFaceLocation->draw(src);
//
//    /*Mat 对象使用完了 必须要释放 */
//    src.release();
//    gray.release();
//    /*JNI 释放*/
//    env->ReleaseByteArrayElements(input_image, _input_image, JNI_OK);
//}