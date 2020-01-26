cmake_minimum_required(VERSION 3.4.1)

#
# OLC/Android user application CMAKE
#
# - Just add subdirectory "lib" and link your code with "androidolc"
# - create the "run.cpp" file to instantiate your class
# - your class will then be started by the launcher
#

# Main C & C++ flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -frtti -fexceptions -g -O0 -Wall")

# OpenGL for Android, select ES2 or ES3 if available
if (${ANDROID_PLATFORM_LEVEL} LESS 12)
    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 \
                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()

elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
    add_definitions("-DDYNAMIC_ES3")
    set(GL3STUB_SRC opengl/gl3stub.c)
    set(OPENGL_LIB GLESv2)

else ()
    set(OPENGL_LIB GLESv3)

endif (${ANDROID_PLATFORM_LEVEL} LESS 12)

message( ${CMAKE_CURRENT_SOURCE_DIR} )
message("HOLA")

add_library(androidolc SHARED IMPORTED GLOBAL)
set_target_properties(androidolc PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../../../build/intermediates/library_and_local_jars_jni/debug/${ANDROID_ABI}/libandroidolc.so)
add_library(olcPGE_portable SHARED IMPORTED GLOBAL)
set_target_properties(olcPGE_portable PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../../../build/intermediates/library_and_local_jars_jni/debug/${ANDROID_ABI}/libolcPGE_portable.so)

#target_include_directories(androidolc PUBLIC ${CMAKE_SOURCE_DIR}/../include)


# Make olcpixelengine headers visible to your program
# include_directories(lib/olcPGE_portable/src)
# include_directories(lib/olcPGE_portable/src/arch/android)

# Add your application source code
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/app)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(include/libpng)
include_directories(include)

# Pack your application files together with the "run.cpp" static intiializer
add_library(olcapp SHARED
        run.cpp
        app/demo/DemoEngine.cpp
        )

# Link you application with Android SDK and AndroidOLC shared Library
target_link_libraries(olcapp
        android
        androidolc
        olcPGE_portable
        )

# Enjoy !