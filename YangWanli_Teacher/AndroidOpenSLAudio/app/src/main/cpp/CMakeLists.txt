cmake_minimum_required(VERSION 3.10.2)

project("androidopenslaudio")

add_library(native-lib
             SHARED
             native-lib.cpp )

find_library(log-lib
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(native-lib
                      OpenSLES
                      ${log-lib} )