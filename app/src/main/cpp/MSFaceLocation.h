
#ifndef MSOPENCV_MSFACELOCATION_H
#define MSOPENCV_MSFACELOCATION_H

#include <android/native_window_jni.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <pthread.h>
#include "log_util.h"
using namespace cv;

///*函数指针声明定义*/
//typedef void(*RenderCallback)(Mat,long);


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

public:
    MSFaceLocation(const char* model);
    ~MSFaceLocation();

public:
    void start();
    void stop();
};


#endif //MSOPENCV_MSFACELOCATION_H
