
#include "MSFaceLocation.h"

MSFaceLocation::MSFaceLocation(const char *model) {
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(model));
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(model));
    //跟踪器
    DetectionBasedTracker::Parameters DetectorParams;
    tracker = makePtr<DetectionBasedTracker>(DetectionBasedTracker(mainDetector, trackingDetector,
                                                                   DetectorParams));
}

MSFaceLocation::~MSFaceLocation() {
}

void MSFaceLocation::start() {
    tracker->run();
}

void MSFaceLocation::stop() {
    tracker->stop();
}

