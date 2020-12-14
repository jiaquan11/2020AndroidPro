cmake_minimum_required(VERSION 3.10.2)

project("mymusic")

include_directories(include)
include_directories(soundtouch/include)
include_directories(soundtouch/SoundTouch)

add_library(native-lib
        SHARED
        native-lib.cpp
        CallJava.cpp
        WLFFmpeg.cpp
        WLAudio.cpp
        WLQueue.cpp
        WLPlayStatus.cpp
        WLPcmBean.cpp
        WLBufferQueue.cpp
        soundtouch/SoundTouch/AAFilter.cpp
        soundtouch/SoundTouch/FIFOSampleBuffer.cpp
        soundtouch/SoundTouch/FIRFilter.cpp
        soundtouch/SoundTouch/cpu_detect_x86.cpp
        soundtouch/SoundTouch/sse_optimized.cpp
        soundtouch/SoundTouch/RateTransposer.cpp
        soundtouch/SoundTouch/SoundTouch.cpp
        soundtouch/SoundTouch/InterpolateCubic.cpp
        soundtouch/SoundTouch/InterpolateLinear.cpp
        soundtouch/SoundTouch/InterpolateShannon.cpp
        soundtouch/SoundTouch/TDStretch.cpp
        soundtouch/SoundTouch/PeakFinder.cpp)

add_library(avcodec-57 SHARED IMPORTED)
set_target_properties(avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libavcodec-57.so)

add_library(avdevice-57 SHARED IMPORTED)
set_target_properties(avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libavdevice-57.so)

add_library(avfilter-6 SHARED IMPORTED)
set_target_properties(avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libavfilter-6.so)

add_library(avformat-57 SHARED IMPORTED)
set_target_properties(avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libavformat-57.so)

add_library(avutil-55 SHARED IMPORTED)
set_target_properties(avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libavutil-55.so)

add_library(postproc-54 SHARED IMPORTED)
set_target_properties(postproc-54
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libpostproc-54.so)

add_library(swresample-2 SHARED IMPORTED)
set_target_properties(swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libswresample-2.so)

add_library(swscale-4 SHARED IMPORTED)
set_target_properties(swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}/libswscale-4.so)

find_library(log-lib
        log)

target_link_libraries(native-lib
        ${log-lib}
        avcodec-57
        avdevice-57
        avfilter-6
        avformat-57
        avutil-55
        postproc-54
        swresample-2
        swscale-4
        OpenSLES
        )