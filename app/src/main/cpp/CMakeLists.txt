cmake_minimum_required(VERSION 3.4.1)

#
# OLC/Android user application CMAKE
#
# (C) 2020 Rodolfo Lopez Pintor.
# License: Creative Commons CC-BY
#
# - Read through the comments.
# - TL/DR : add your cpp files to the library at the bottom
#           OLC header files will be recognized in the ide.
#           To instantiate your Main Engine class, take a look at the file run.cpp.
#
# This script works in combination with build.gradle. Build.Gradle unpacks the .so libraries
# and header files that we need here, and publishes the headers them so are available to your source files.
# Beware that the header files are not the ones used by the engine, just convenience headers packed for linking
# and development.
#
# If you need to update the olcPixelEngine.h, the place to do it is the core project. Your modifications there
# will automagically propagate here as soon as you replace the release .AAR file into the olcpge_android folder
# in this project.
#

# Main C & C++ flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -frtti -fexceptions -g -O0 -Wall")

# OpenGL for Android, autoselect ES2 or ES3 if available
if (${ANDROID_PLATFORM_LEVEL} LESS 12)
    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 \
                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()

elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
    add_definitions("-DDYNAMIC_ES3")
    set(OPENGL_LIB GLESv2)

else ()
    set(OPENGL_LIB GLESv3)

endif (${ANDROID_PLATFORM_LEVEL} LESS 12)

message("PIXengine/Android - Rodolfo Lopez Pintor 2020")

# Declare the OLC libaries. These libraries come packaged inside the AAR
# The Build.Gradle script contains logic to unzip the libraries
# and place them in jniLibs so we can use them here. This has been a time
# consuming investigation as there are a lot of stones in the road if you
# need to manipulate or access the AAR *before* CMAKE invocation with the new
# versions of the gradle plugin that are a little paranoid about dependencies
# visibility. This should really be automatic, but unfortunately support of .so in
# library projects is mostly designed for JAVA rather than CMAKE

add_library(pixEngine_launcher SHARED IMPORTED GLOBAL)
set_target_properties(pixEngine_launcher PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libpixEngine_launcher.so)
add_library(pixEngine_core SHARED IMPORTED GLOBAL)
set_target_properties(pixEngine_core PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libpixEngine_core.so)

# Likewise, the headers come packaged in the AAR under /res/raw and the GRADLE
# script conveniently unpacks them. So we make them visible to your program with
# the right namespaces

include_directories(../headers/arch/android/libpng)
include_directories(../headers/core)
include_directories(../headers/input)
include_directories(../headers/items)
include_directories(../headers)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/app)

# Now, just add your application source code together with the "run.cpp" to launch the main class.
# The library name has to be "olcapp" as this is hardcoded into the JAVA launcher.
add_library(pixapp
        SHARED
        run.cpp
        app/demo/DemoEngine.cpp
#       app/videos/OneLoneCoder_PGE_Balls2.h
#       app/videos/OneLoneCoder_PGE_PathFinding_WaveProp.cpp
#       app/videos/OneLoneCoder_PGE_PolygonCollisions1.cpp
        )
# And finally, link you application with Android SDK and AndroidOLC pre-compiled libraries
target_link_libraries(pixapp
        android
        log
        pixEngine_core
        pixEngine_launcher
        )

# Enjoy !
