
#ifndef MSOPENCV_LOG_UTIL_H
#define MSOPENCV_LOG_UTIL_H
#include <android/log.h>

#define LOG_TAG "lpf"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#endif //MSOPENCV_LOG_UTIL_H
