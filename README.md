# 📏 DepthFinder  

Using sensors to estimate the depth (distance) of an object.  
This project demonstrates two different approaches to depth measurement:

1. **Ultrasonic Sensor** – Direct distance measurement using sound waves  TODO. ( Since I dont have the hardware at the moment ).
2. **Stereo Camera (Vision-Based)** – Depth estimation using disparity between two images  

---

## 🎥 Camera Setup Used in This Project

For stereo‑vision testing, **two USB cameras** were used:

- Both cameras were connected via USB ports  
- Positioned side‑by‑side with a fixed baseline  
- Captured synchronized left and right images manually  
- Used as input for VPI Stereo Disparity computation  

This setup is ideal for experimentation and testing on Jetson boards without requiring a dedicated stereo module.

---

## 🧭 Overview

Depth estimation is a fundamental requirement in robotics, automation, and computer vision.  
This project explores **two complementary methods**:

### ✔ Ultrasonic Sensor  
A simple and reliable method that measures distance using the time-of-flight of sound waves.

### ✔ Stereo Camera (Two USB Cameras)  
A more advanced method that uses two images captured from slightly different viewpoints.  
By computing **disparity**, we can estimate the depth of each pixel in the scene.

---

## 🛠 Hardware Used

### **1. Two USB Cameras**
- Plug‑and‑play  
- No special drivers required  
- Works with V4L2, OpenCV, and GStreamer  
- Used to capture left and right frames for disparity testing  

### **2. Ultrasonic Sensor (HC-SR04 or equivalent)**
- Trigger + Echo pins  
- Measures distance using sound wave reflection  

### **3. NVIDIA Jetson Board**
- Jetson Xavier NX / AGX Xavier  
- Supports:
  - CUDA  
  - VPI  
  - OpenCV  
  - DeepStream  
  - GStreamer NVMM  

---

## 🧰 Software Stack

- **VPI (Vision Programming Interface)** – Stereo disparity computation  
- **CUDA Toolkit** – GPU acceleration  
- **OpenCV** – Image capture, processing, visualization  
- **GStreamer + NVIDIA Plugins** – USB camera capture  
- **Jetson Multimedia API** – Zero-copy pipelines  
- **CMake + GCC** – Build system  

---

## 🎯 Features

- Capture images from two USB cameras  
- Wrap frames into VPI images  
- Compute disparity using CUDA backend  
- Convert disparity to depth map  
- Save output as `disparity.png`  
- Ultrasonic sensor depth measurement (optional module)  

---

## 🚀 Running the Stereo Disparity Test

```bash
mkdir build && cd build
cmake ..
make -j4
./depthfinder

