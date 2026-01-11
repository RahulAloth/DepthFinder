cmake_minimum_required(VERSION 3.10)
project(adso_nvmm)

find_package(PkgConfig REQUIRED)
find_package(VPI REQUIRED)
find_package(OpenCV REQUIRED)

# Find VPI 3 library (actual name is libnvvpi.so)
find_library(VPI_LIB nvvpi
    PATHS
        /opt/nvidia/vpi3/lib/aarch64-linux-gnu
        /usr/lib/aarch64-linux-gnu
)

# -----------------------------
# GStreamer packages
# -----------------------------
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)
pkg_check_modules(GST_ALLOC REQUIRED gstreamer-allocators-1.0)


# -----------------------------
# Include directories
# -----------------------------
include_directories(
    ${GST_INCLUDE_DIRS}
    ${GST_APP_INCLUDE_DIRS}
    ${GST_VIDEO_INCLUDE_DIRS}
    ${GST_ALLOC_INCLUDE_DIRS}
    ${VPI_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

# -----------------------------
# Library directories
# -----------------------------
link_directories(
    ${GST_LIBRARY_DIRS}
    ${GST_APP_LIBRARY_DIRS}
    ${GST_VIDEO_LIBRARY_DIRS}
    ${GST_ALLOC_LIBRARY_DIRS}
    ${VPI_LIBRARY_DIRS}
    ${OpenCV_LIBS}
)

# -----------------------------
# Build target
# -----------------------------
add_executable(usb_cam src/main.cpp)

# -----------------------------
# Link libraries
# -----------------------------
target_link_libraries(usb_cam
    ${GST_LIBRARIES}
    ${GST_APP_LIBRARIES}
    ${GST_VIDEO_LIBRARIES}
    ${GST_ALLOC_LIBRARIES}
    ${VPI_LIB}
    ${OpenCV_LIBS} 
    EGL
)
