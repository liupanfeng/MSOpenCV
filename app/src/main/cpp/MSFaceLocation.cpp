
#include "MSFaceLocation.h"

MSFaceLocation::MSFaceLocation(const char *model) {
    /*初始化互斥锁*/
    pthread_mutex_init(&mutex,nullptr);
    /*加载模型*/
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(model));
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(model));

    /*初始化跟踪器*/
    DetectionBasedTracker::Parameters DetectorParams;
    tracker = makePtr<DetectionBasedTracker>(DetectionBasedTracker(mainDetector, trackingDetector,
                                                                   DetectorParams));
}

MSFaceLocation::~MSFaceLocation() {
    pthread_mutex_destroy(&mutex);
    /*调动宏函数释放window指针*/
    A_NATIVE_WINDOW_RELEASE(window);
}


void MSFaceLocation::setANativeWindow(ANativeWindow *window) {
    pthread_mutex_lock(&mutex);
    /*释放之前的*/
    A_NATIVE_WINDOW_RELEASE(window);
    /*换成最新的*/
    this->window=window;
    pthread_mutex_unlock(&mutex);
}

void MSFaceLocation::draw(Mat src) {
    pthread_mutex_lock(&mutex);
    do{
        if (!window){
            break;
        }
        /*根据Mat 初始化window 宽高*/
        ANativeWindow_setBuffersGeometry(window,src.cols,src.rows,WINDOW_FORMAT_RGBA_8888);
        ANativeWindow_Buffer buffer;
        if (ANativeWindow_lock(window,&buffer,nullptr)){
            /*如果被锁住了，直接释放*/
            A_NATIVE_WINDOW_RELEASE(window)
            break;
        }
        uint8_t *dest_data=static_cast<uint8_t *>(buffer.bits);
        int dstlineSize = buffer.stride * 4;
        uint8_t *srcData = src.data;
        int srclineSize = src.cols * 4;
        for (int i = 0; i < buffer.height; ++i) {
            memcpy(dest_data + i * dstlineSize, srcData + i * srclineSize, srclineSize);
        }
        /*解锁 并刷新*/
        ANativeWindow_unlockAndPost(window);

    } while (false);
    pthread_mutex_unlock(&mutex);
}

void MSFaceLocation::start() {
    tracker->run();
}

void MSFaceLocation::stop() {
    tracker->stop();
}
