# Optical Flow

Optical flow refers to the pattern of apparent motion of objects, surfaces, and edges in a visual scene, caused by the relative motion between an observer (camera) and the scene. It is a core concept in computer vision and is widely used in applications such as object tracking, video stabilization, motion segmentation, autonomous driving, and video compression.

---

## Fundamentals

- **Optical flow** describes how pixel intensities move between two consecutive frames of a video.
- It is represented as a **2D vector field**, where each vector indicates the displacement (motion) of a pixel from frame *t* to frame *t+1*.
- The motion field is computed using **two consecutive images**.
- Optical flow assumes:
  - **Brightness constancy**: pixel intensity does not change between frames.
  - **Small motion**: displacement between frames is small.
  - **Spatial smoothness**: neighboring pixels have similar motion.

There are two main categories of optical flow estimation:

1. **Dense Optical Flow**
2. **Sparse Optical Flow**

---

## Dense Optical Flow

Dense optical flow computes the motion vector **for every pixel** in the image.

### Characteristics
- Produces a full-resolution motion field.
- Useful for:
  - Motion segmentation
  - Video compression
  - Background subtraction
  - Scene understanding
- Common algorithms:
  - **Horn–Schunck**
  - **Farnebäck**
  - **Deep learning–based methods** (e.g., FlowNet, RAFT)

### Pros
- Complete motion information.
- Good for global motion analysis.

### Cons
- Computationally expensive.
- Sensitive to noise and illumination changes.

---

## Sparse Optical Flow

Sparse optical flow computes motion **only for selected keypoints**.

### Characteristics
- Tracks a subset of pixels (usually corners or features).
- Often uses feature detectors such as:
  - **Harris corners**
  - **Shi–Tomasi**
  - **FAST**
- Common algorithm:
  - **Lucas–Kanade (LK)** method

### Pros
- Much faster than dense flow.
- Works well for tracking objects or features.
- Robust to noise.

### Cons
- Does not provide motion for the entire image.
- Depends heavily on feature quality.

---

## Resolution Pyramid (Image Pyramid)

Optical flow often struggles with **large motions** because the assumption of “small displacement” breaks down.  
To solve this, we use a **multi-scale (coarse-to-fine) pyramid**.

### How the Pyramid Works

1. **Build a pyramid** of the input images:
   - Level 0: full resolution  
   - Level 1: half resolution  
   - Level 2: quarter resolution  
   - … and so on.

2. **Start at the lowest resolution** (coarsest level):
   - Motion is small relative to the image size.
   - Compute optical flow here first.

3. **Upsample the flow** to the next higher resolution.

4. **Warp the next-level image** using the upsampled flow:
   - This reduces the remaining motion.
   - Optical flow is refined at this level.

5. **Repeat** until reaching the highest resolution.

6. The final refined flow at the top level is the **final optical flow estimate**.

### Why It Works
- Large motions become small at coarse levels.
- Reduces computational cost.
- Improves accuracy and stability.

---

## Summary

| Method | Description | Pros | Cons |
|--------|-------------|------|------|
| **Dense Optical Flow** | Computes motion for all pixels | Complete motion field | Slow, sensitive to noise |
| **Sparse Optical Flow** | Tracks selected keypoints | Fast, robust | Limited coverage |
| **Pyramid Approach** | Multi-scale refinement | Handles large motion | More complex |

---
