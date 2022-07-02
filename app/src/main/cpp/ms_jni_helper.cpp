
#include "ms_jni_helper.h"
#include <jni.h>
#include <string>

ANativeWindow *window=0;
/*静态初始化锁*/
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void renderCallback(Mat img){
    pthread_mutex_lock(&mutex);
    do {
        if (!window) {
            LOGI("window is null");
            break;
        }
        ANativeWindow_setBuffersGeometry(window, img.cols, img.rows,
                                         WINDOW_FORMAT_RGBA_8888);
        ANativeWindow_Buffer buffer;
        if (ANativeWindow_lock(window, &buffer, nullptr)) {
            ANativeWindow_release(window);
            window = nullptr;
            break;
        }
        uint8_t *dstData = static_cast<uint8_t *>(buffer.bits);
        int dstlineSize = buffer.stride * 4;
        uint8_t *srcData = img.data;
        int srclineSize = img.cols * 4;
        for (int i = 0; i < buffer.height; ++i) {
            memcpy(dstData + i * dstlineSize, srcData + i * srclineSize, srclineSize);
        }
        ANativeWindow_unlockAndPost(window);
    } while (false);

    pthread_mutex_unlock(&mutex);
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_meishe_msopencv_MSFaceLocation_nativeCreateObject(JNIEnv *env, jobject thiz,
                                                           jstring model) {
    const char *_model = env->GetStringUTFChars(model, JNI_OK);
    MSFaceLocation *msFaceLocation = new MSFaceLocation(_model);
    env->ReleaseStringUTFChars(model, _model);
    LOGI("nativeCreateObject success");
    return (jlong) msFaceLocation;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_msopencv_MSFaceLocation_nativeSetSurface(JNIEnv *env, jobject thiz,
                                                         jlong m_native_obj, jobject surface) {

    pthread_mutex_lock(&mutex);
    /*先释放之前的显示窗口*/
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    /*创建新的窗口用于视频显示*/
    window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutex);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_msopencv_MSFaceLocation_nativeRelease(JNIEnv *env, jclass clazz, jlong thiz) {

    if (thiz != 0) {
        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
        msFaceLocation->tracker->stop();
        delete msFaceLocation;
    }

    pthread_mutex_lock(&mutex);
    /*释放显示的窗口*/
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    pthread_mutex_unlock(&mutex);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_msopencv_MSFaceLocation_nativeStart(JNIEnv *env, jclass clazz, jlong thiz) {
    if (thiz != 0) {
        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
        LOGI("nativeStart start run");
        msFaceLocation->start();
        LOGI("nativeStart run success");
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_msopencv_MSFaceLocation_nativeStop(JNIEnv *env, jclass clazz, jlong thiz) {
    if (thiz != 0) {
        MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
        msFaceLocation->stop();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_msopencv_MSFaceLocation_nativeDetect(JNIEnv *env, jclass clazz, jlong thiz,
                                                     jbyteArray input_image, jint width,
                                                     jint height, jint rotation_degrees) {
    if (thiz == 0) {
        return;
    }
    MSFaceLocation *msFaceLocation = reinterpret_cast<MSFaceLocation *>(thiz);
    jbyte *_input_image = env->
            GetByteArrayElements(input_image, JNI_OK);
    LOGI("with=%d,height=%d",width,height);
    /*cameraX 传递过来的数据是i420*/
    Mat src(height * 3 / 2, width, CV_8UC1, _input_image);
    /*yuv420 转 rgba i420*/
    cvtColor(src, src, CV_YUV2RGBA_I420);
    if (rotation_degrees == 90) {
        /*正向*/
        rotate(src, src, ROTATE_90_CLOCKWISE);
    } else if (rotation_degrees == 270) {
        /*反向*/
        rotate(src, src, ROTATE_90_COUNTERCLOCKWISE);
    }
    /*镜像选装*/
//    flip(src,src,1);
    Mat gray;
    cvtColor(src, gray, CV_RGBA2GRAY);
    equalizeHist(gray, gray);
    msFaceLocation->tracker->process(gray);

    std::vector<Rect> faces;
    msFaceLocation->tracker->getObjects(faces);
    for (Rect face:faces) {
        rectangle(src, face, Scalar(255, 0, 255));
    }
    renderCallback(src);

    /*Mat 对象使用完了 必须要释放 */
    src.release();
    gray.release();
    /*JNI 释放*/
    env->ReleaseByteArrayElements(input_image, _input_image, JNI_OK);
}


