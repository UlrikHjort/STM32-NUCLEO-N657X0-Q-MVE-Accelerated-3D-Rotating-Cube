# STM32 NUCLEO-N657X0-Q MVE-Accelerated 3D Rotating Cube 

This project implements a real-time **3D wireframe rotating cube** optimized for **ARM Cortex-M processors with MVE (Helium SIMD)**. It demonstrates how ARM MVE intrinsics can be used to accelerate 3D math operations such as rotation and transformation on embedded systems, while rendering to a **480×320 LCD display**.


---

## Features

- Real-time animated 3D wireframe cube  
- Perspective projection  
- Independent X, Y, and Z rotation  
- ARM MVE (Helium) SIMD–accelerated math  
- Bresenham line rasterization  

---

## Target Platform

- **CPU:** ARM Cortex-M with MVE (Helium), e.g. Cortex-M55 / Cortex-M85  
- **Display:** 480×320  3.5 inch Arduino 8BIT Module MAR3501 Display

---

## Rendering Pipeline

Each frame follows a classic 3D rendering pipeline:

1. Define cube vertices in object space  
2. Scale the cube to world size  
3. Rotate around X, Y, and Z axes using MVE SIMD  
4. Apply perspective projection  
5. Draw cube edges using line rasterization  
6. Update rotation angles for animation  

---

## Cube Geometry

The cube is defined by **8 normalized vertices** centered at the origin, ranging from `-1` to `+1` on each axis. The cube edges are defined using index pairs, forming the 12 edges of a wireframe cube.

---

## SIMD-Friendly Data Layout

To efficiently use MVE, the cube vertices are converted from an **Array of Structures (AoS)** to a **Structure of Arrays (SoA)** layout each frame:

- Separate arrays for X, Y, and Z coordinates  
- Allows processing of **4 vertices at a time**  
- Avoids non-contiguous memory loads  

---

## MVE-Optimized 3D Rotation

Rotation around each axis is implemented using ARM MVE intrinsics. Each rotation processes four vertices in parallel using fused multiply-add and fused multiply-subtract operations.


ARM MVE does not support floating-point division, so the division is performed using scalar code per vertex, while multiplication and offset are vectorized.

---

## Line Rendering

All cube edges are rendered using a classic **Bresenham line algorith/*
 * Key MVE intrinsics used:
 * - vld1q_f32 / vst1q_f32: Load/store 4 floats
 * - vmulq_f32: Multiply 4 floats in parallel
 * - vfmaq_f32: Fused multiply-add (a + b*c)
 * - vfmsq_f32: Fused multiply-subtract (a - b*c)
 * - vdupq_n_f32: Broadcast scalar to all lanes
 * - vcvtq_s32_f32: Convert float to int32
 * - vgetq_lane_f32 / vgetq_lane_s32: Extract lane from vector
 *
 * Performance: The rotation calculations benefit most from MVE (3x faster),
 * while projection uses scalar division (MVE has no fdiv). Overall speedup
 * is still ~2x compared to pure scalar code.
 */
m**:

- Integer-only arithmetic  
- Pixel-accurate output  
- Screen bounds checking  


---

## Animation Loop

For each frame:

1. Clear the display  
2. Scale and copy cube vertices into SIMD-friendly arrays  
3. Apply rotations around X, Y, and Z  
4. Project vertices into 2D screen space  
5. Draw all cube edges  
6. Increment rotation angles  

Rebuilding the cube every frame prevents cumulative floating-point drift.

---

## Performance

- Rotation math is approximately **3× faster** than scalar code  
- Projection performance is limited by scalar division  
- Overall performance improvement is approximately **2×** compared to a non-SIMD implementation  

---

## Building

Full project was build in STM32CubeIde. Create a project for STM32N657X0HXQ and copy the files in this repro to Core/src and Core/Inc. Project depends on my display driver for the  MAR3501 Display. It is included here too. For details about the driver, see the repro with the display driver project. 