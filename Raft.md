# RAFT / RAFT‑Stereo
I decided to use RAFT Algorithm for estimating Sterio depth I mean the disparity Calculation. 

Please see below step by step guide for the RAFT Algorithm implimentation. The Documentation is available here:

RAFT : https://arxiv.org/pdf/2109.07547

## Steps:

## 1. Convert the images into feature maps
RAFT does not work directly on raw pixels.  
It first uses a CNN to turn both the left and right images into **feature maps**.  
These feature maps capture useful information like edges, textures, and patterns.

A separate **context feature map** is also extracted from the left image.  
This context stays fixed and helps guide the refinement process.

---

## 2. Build the correlation pyramid (a memory of matching scores)
Before estimating disparity, RAFT computes how well each pixel in the left image
matches with pixels in the right image at different disparities.

This creates a **large table of matching scores**.

Then RAFT downsamples this table to multiple scales, forming a
**correlation pyramid**.

This pyramid acts like a **memory**:
- It stores all matching information.
- It is computed once.
- It is reused throughout the entire refinement process.

---

## 3. Start with a simple initial disparity guess
RAFT begins with a very basic starting point:
- Disparity = 0 everywhere.

No fancy initialization — just a neutral guess.

---

## 4. Enter the iterative refinement loop
This is the core of RAFT.  
It improves the disparity step by step, over many iterations.

Each iteration does the following:

---

### 4.1 Look up correlation values using the current disparity
For each pixel:
- RAFT takes the current disparity estimate.
- It uses this value to **look up matching scores** from the correlation pyramid.
- It also checks nearby disparities (a small window around the current guess).
- It does this at multiple pyramid levels (coarse to fine).

These looked‑up values form a **small correlation feature vector**.

---

### 4.2 Combine correlation features with context and disparity features
RAFT also:
- Processes the current disparity estimate with a small CNN.
- Uses the fixed context features extracted earlier.

These are combined with the correlation features.

---

### 4.3 Feed everything into a GRU
A GRU is a type of recurrent neural network with memory.

At each iteration, the GRU:
- Reads the new features.
- Updates its internal hidden state.
- Decides how disparity should change.

This is where RAFT’s “intelligence” lives.

---

### 4.4 Predict a small update to disparity
The GRU outputs a **small correction** to the current disparity.

RAFT adds this correction to the previous estimate, making the disparity more accurate.

---

## 5. Repeat the refinement many times
RAFT repeats the loop (lookup → GRU → update) around **32 times**.

Each iteration:
- Looks up better correlation values.
- Makes a better correction.
- Refines the disparity further.

By the end, the disparity map becomes very accurate.

---

## 6. Upsample to full resolution
RAFT predicts disparity at a lower resolution (like 1/4 or 1/8 of the image).

To get full resolution:
- RAFT uses a learned upsampling method.
- It blends nearby low‑resolution disparities using predicted weights.

This produces a smooth, high‑resolution disparity map.

---


---

## Final Summary
RAFT builds a memory of matching scores between the left and right images (the correlation pyramid).  
It starts with a simple disparity guess and repeatedly refines it.  
At each step, it looks up correlation values based on the current disparity, combines them with context features, and feeds everything into a GRU.  
The GRU updates its internal memory and predicts a small correction to the disparity.  
After many iterations, RAFT produces a highly accurate disparity map, which you can convert into depth and point clouds.


---

## RAFT Algorithm

Algorithm: RAFT / RAFT-Stereo (High-Level, Step-by-Step)

Inputs:
  - Left image: I_L
  - Right image: I_R
  - Camera intrinsics (for later depth/point cloud, optional)

Outputs:
  - Disparity map: D_final

------------------------------------------------------------
1. Feature Extraction
------------------------------------------------------------
1.1 Extract features from both images using a CNN:
    F_L = FeatureEncoder(I_L)
    F_R = FeatureEncoder(I_R)

1.2 Extract context features from the left image:
    C = ContextEncoder(I_L)

------------------------------------------------------------
2. Correlation Pyramid Construction (Precomputed Once)
------------------------------------------------------------
2.1 Compute all-pairs correlation (along horizontal / disparity axis):
    Corr_0 = Correlate(F_L, F_R)
    # Corr_0: [H, W, D] similarity volume

2.2 Build a multi-scale correlation pyramid by downsampling:
    Corr_1 = Downsample(Corr_0)
    Corr_2 = Downsample(Corr_1)
    Corr_3 = Downsample(Corr_2)

2.3 Store:
    CorrPyramid = [Corr_0, Corr_1, Corr_2, Corr_3]

------------------------------------------------------------
3. Initialization
------------------------------------------------------------
3.1 Initialize disparity estimate:
    D_0 = zeros_like(low_res_grid)   # e.g., 1/8 or 1/4 resolution

3.2 Initialize GRU hidden states (multi-resolution in RAFT-Stereo):
    H_1/32 = zeros(...)
    H_1/16 = zeros(...)
    H_1/8  = zeros(...)

------------------------------------------------------------
4. Iterative Refinement Loop
------------------------------------------------------------
For iteration i = 1 to N:

  4.1 Correlation Lookup using Current Disparity
      - For each pixel (x, y) at highest GRU resolution:
          - Take current disparity: d_i(x, y)
          - For each level L in CorrPyramid:
              - Sample correlation values at:
                    d_i(x, y) + offsets_L
                using linear interpolation along disparity axis
          - Concatenate all sampled values across levels and offsets:
                CorrFeat(x, y) = Concat(samples from all levels)

  4.2 Prepare Inputs for GRU
      - Process current disparity estimate with small convs:
            DispFeat = ConvDisp(D_i)
      - Use context features C (precomputed, fixed):
            CtxFeat = C (possibly cropped / resampled)

      - Form GRU input at highest resolution:
            GRU_in_1/8 = Concat(CorrFeat, DispFeat, CtxFeat)

  4.3 Multi-Resolution GRU Update (RAFT-Stereo)
      - Update low-res GRUs first (e.g., 1/32, 1/16):
            H_1/32 = GRU_1/32(H_1/32, downsample(GRU_in_1/8), ...)
            H_1/16 = GRU_1/16(H_1/16, up/downsample(H_1/32), ...)
      - Update highest-res GRU (1/8), using cross-connected states:
            H_1/8  = GRU_1/8(H_1/8, GRU_in_1/8, upsample(H_1/16), ...)

  4.4 Predict Disparity Update
      - From H_1/8, predict a small update:
            ΔD_i = Head(H_1/8)

  4.5 Update Disparity Estimate
      - D_{i+1} = D_i + ΔD_i

End For

------------------------------------------------------------
5. Upsampling to Full Resolution
------------------------------------------------------------
5.1 Use learned convex upsampling:
    - For each high-res pixel:
        - Take 3x3 neighborhood of low-res disparities around it
        - Predict weights W (sum to 1, all ≥ 0)
        - D_full(x, y) = Σ W_k * D_low(neighbor_k)

5.2 Output:
    D_final = D_full

------------------------------------------------------------
6. (Optional) Depth and Point Cloud (Your Pipeline)
------------------------------------------------------------
6.1 Convert disparity to depth:
    Z = f * B / D_final

6.2 Project to 3D using camera intrinsics:
    X, Y, Z = PixelTo3D(D_final, intrinsics)


6.3 Form point cloud:
    P = {(X, Y, Z) for all valid pixels}
    
## Mini RAFT.

- We are using Jetson nano so a mini Raft methode would be enough.
### Algorithm .
- Your stereo algorithm should have only 5 components:
- ✔ 1. Lightweight Encoder

- Use ConvNeXt‑Tiny, MobileNetV3, or EfficientNet‑Lite  
- → Fast, modern, optimized for TensorRT.
- ✔ 2. 1D Correlation Layer

- Use PyTorch’s built‑in ops (no custom CUDA).
- Simple sliding window correlation.
- ✔ 3. ConvGRU (single layer)

- Use modern PyTorch with fused kernels.
- One GRU only — not 3.
- ✔ 4. Flow Head

- Simple 2‑layer CNN.
- ✔ 5. Upsampling

- Use F.interpolate or a tiny learned mask.

- This gives you:

    - 20–40 FPS on Jetson Orin

    - ONNX‑friendly

    - TensorRT‑friendly

    - Easy to train

    - Easy to maintain

