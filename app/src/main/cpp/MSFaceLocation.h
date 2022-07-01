
#ifndef MSOPENCV_MSFACELOCATION_H
#define MSOPENCV_MSFACELOCATION_H

#include <android/native_window_jni.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <pthread.h>

using namespace cv;


#define A_NATIVE_WINDOW_RELEASE(window)   \
if (window) { \
            ANativeWindow_release(window); \
            window = 0; \
      }

/**
 * 人脸跟随适配器
 */
class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {
        CV_Assert(detector);
    }

    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {

        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);

    }

    virtual ~CascadeDetectorAdapter() {

    }

private:
    CascadeDetectorAdapter();

    cv::Ptr<cv::CascadeClassifier> Detector;
};




class MSFaceLocation {

public:
    /**
     * 人脸追踪
     */
    Ptr<DetectionBasedTracker> tracker;
    /**
     * 互斥锁
     */
    pthread_mutex_t mutex;
    /**
     * 画面渲染
     */
    ANativeWindow *window = 0;

public:
    MSFaceLocation(const char* path);
    ~MSFaceLocation();

public:
    void setANativeWindow(ANativeWindow *window);
    void draw(Mat src);

    void start();

    void stop();
};


#endif //MSOPENCV_MSFACELOCATION_H
