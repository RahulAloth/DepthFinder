# 🚀 Jetson Vision Processing Environment  
*Installations used for Disparity Calculations and Hardware Platform*

---

## 🟩 1. Hardware Platform

System: J4011 reComputer Industrial Jetson Orin NX 8 GB, 70 TOPS.

### **NVIDIA Jetson Orin‑class board**
- ARM64 architecture (`/usr/lib/aarch64-linux-gnu/`)
- JetPack / L4T components present
- Supports DeepStream 7.x, VPI 2.x/3.x, CUDA 12.x  
- Supports OFA (Optical Flow Accelerator) and NVMM pipelines

This matches:

- **Jetson Xavier NX**  

---

## 🟦 2. Core Software Stack

### **JetPack Components Installed**
- `nvidia-l4t-core`
- `nvidia-l4t-jetson-multimedia-api`
- `/etc/nv_tegra_release` present
- CUDA Toolkit installed
- VPI libraries installed
- DeepStream SDK installed

JetPack provides:

- CUDA  
- cuDNN  
- TensorRT  
- VPI  
- Multimedia API  
- GStreamer NVIDIA plugins  

---

## 🟧 3. Vision Programming Interface (VPI)

Installed packages and files:

- `vpi2-dev`
- `vpi2-runtime`
- `vpi1-dev` (older version also present)
- `/usr/include/vpi/`
- `/usr/lib/aarch64-linux-gnu/libvpi*`
- `vpi_version` command works

### **VPI Features Enabled**
- CUDA backend  
- CPU backend  
- OpenCV interop  
- Stereo Disparity Estimator  
- NVMM / EGLImage support  

Used for:

- Stereo disparity (SGM)
- Image wrapping (`vpiImageCreateWrapperOpenCVMat`)
- CUDA‑accelerated image processing

---

## 🟪 4. CUDA Toolkit

Installed via:

- `nvidia-cuda-toolkit`
- `cuda-keyring`
- `sudo apt install cuda`
- Verified with `nvcc --version`

### **CUDA Capabilities**
- CUDA 12.x toolchain  
- GPU acceleration for VPI  
- GPU memory management  
- CUDA streams  

---

## 🟨 5. OpenCV

Installed:

- `libopencv-dev`
- `python3-opencv`
- CUDA-enabled OpenCV modules present (`libopencv_cuda*`)

### **OpenCV Usage**
- Loading PNG/JPEG images  
- Converting to grayscale  
- Wrapping into VPI images  
- Saving disparity maps  
- Debug visualization  

---

## 🟥 6. GStreamer + NVIDIA Plugins

Installed:

### **GStreamer Core**
- `gstreamer1.0-tools`
- `gstreamer1.0-plugins-base`
- `gstreamer1.0-plugins-good`
- `gstreamer1.0-plugins-bad`
- `gstreamer1.0-libav`

### **NVIDIA Accelerated Plugins**
- `nvv4l2decoder`
- `nvvidconv`
- `nvvideoconvert`
- `nveglglessink`
- `nvstreammux`
- `nvv4l2camerasrc`

### **Purpose**
- Hardware-accelerated camera capture  
- NVMM → CUDA → VPI zero-copy pipelines  
- Real-time video processing  
- DeepStream integration  

---

## 🟫 7. DeepStream SDK

Installed versions:

- **DeepStream 7.0**
- **DeepStream 7.1**

Also copied:

- Sample configs  
- Sample models  
- Primary detector configs  

### **DeepStream Usage**
- AI inference  
- Multi-stream pipelines  
- NVMM buffer management  
- GStreamer integration  

Not required for disparity, but extremely useful for full pipelines.

---

## 🟩 8. Jetson Multimedia API

Installed:

- `nvidia-l4t-jetson-multimedia-api`
- `/usr/src/jetson_multimedia_api/include/nvbuf_utils.h`

### **Capabilities**
- NvBuffer  
- NvBufSurface  
- DMA-BUF  
- EGLImage  
- Zero-copy camera → CUDA → VPI  

Essential for high-performance camera pipelines.

---

## 🟦 9. Build Tools

Installed:

- `build-essential`
- `cmake`
- `gdb`
- `pkg-config`
- `git`

### **Purpose**
Required to compile:

- VPI C++ applications  
- CUDA code  
- OpenCV projects  
- DeepStream plugins  

---

## 🟧 10. Camera Tools

Installed:

- `v4l-utils`
- `v4l2-ctl`

### **Purpose**
- Listing cameras  
- Checking supported formats  
- Debugging USB cameras  

---

# ✔️ Final Summary

Your Jetson system is fully equipped for:

### **Stereo Disparity (VPI CUDA backend)**
- VPI 2.x  
- CUDA Toolkit  
- OpenCV  
- Jetson Multimedia API  

### **Real-time camera pipelines**
- GStreamer + NVIDIA plugins  
- NVMM zero-copy  
- DeepStream SDK  

### **Development & debugging**
- CMake  
- GCC  
- GDB  
- v4l2 tools  

This is a complete, production-ready environment for advanced stereo vision, CUDA processing, and embedded Jetson pipelines.

