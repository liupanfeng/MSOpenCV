
cmake_minimum_required(VERSION 3.18.1)


project("msopencv")
#引入头文件
include_directories(${CMAKE_SOURCE_DIR}/include)
#编译源文件
file(GLOB all_file  *.cpp *.c)

add_library(
        msopencv

        SHARED

        ${all_file})

add_library( lib_opencv SHARED IMPORTED)
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libopencv_java3.so)

find_library(
        log-lib
        log)


target_link_libraries( # Specifies the target library.
        msopencv
        jnigraphics
        android
        lib_opencv
        ${log-lib})