cmake_minimum_required(VERSION 3.10.2)

project("livepusher")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_CRYPTO")

add_library( wlpush
             SHARED
             wlPush.cpp
             WLQueue.cpp
             RtmpPush.cpp
             CallJava.cpp
             librtmp/amf.c
             librtmp/hashswf.c
             librtmp/log.c
             librtmp/parseurl.c
             librtmp/rtmp.c)

find_library( log-lib
              log )

target_link_libraries( wlpush
                       ${log-lib} )