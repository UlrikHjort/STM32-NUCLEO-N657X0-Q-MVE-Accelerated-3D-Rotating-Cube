/***************************************************************************
;                           Rotating Cube MVE
;
;           Copyright (C) 2026 By Ulrik Hørlyk Hjort
;
; Permission is hereby granted, free of charge, to any person obtaining
; a copy of this software and associated documentation files (the
; "Software"), to deal in the Software without restriction, including
; without limitation the rights to use, copy, modify, merge, publish,
; distribute, sublicense, and/or sell copies of the Software, and to
; permit persons to whom the Software is furnished to do so, subject to
; the following conditions:
;
; The above copyright notice and this permission notice shall be
; included in all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
; EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
; LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
; OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
; WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; ***************************************************************************/
#include <math.h>
#include <stdint.h>
#include <arm_mve.h>
#include <stdlib.h>
#include "display.h"

// Display dimensions
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320
#define CENTER_X (SCREEN_WIDTH / 2)
#define CENTER_Y (SCREEN_HEIGHT / 2)

// Cube parameters
#define CUBE_SIZE 50.0f
// Distance from camera to cube
#define DISTANCE 200.0f

// 3D Point structure
typedef struct {
    float x, y, z;
} Point3D;

// 2D Point structure
typedef struct {
    int16_t x, y;
} Point2D;

// Cube vertices (8 corners)
static Point3D cube_vertices[8] = {
    {-1, -1, -1},  // 0
    { 1, -1, -1},  // 1
    { 1,  1, -1},  // 2
    {-1,  1, -1},  // 3
    {-1, -1,  1},  // 4
    { 1, -1,  1},  // 5
    { 1,  1,  1},  // 6
    {-1,  1,  1}   // 7
};

// Cube edges (12 lines connecting vertices)
static const uint8_t cube_edges[12][2] = {
    // Back face
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    // Front face
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    // Connecting edges
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

// Projected vertices
static Point2D projected[8];

// Separate arrays for SIMD processing (Structure of Arrays)
static float x_coords[8] __attribute__((aligned(16)));
static float y_coords[8] __attribute__((aligned(16)));
static float z_coords[8] __attribute__((aligned(16)));

// Rotation angles
static float angle_x = 0.0f;
static float angle_y = 0.0f;
static float angle_z = 0.0f;

void draw_line(uint16_t x0, uint16_t y0,
              uint16_t x1, uint16_t y1,
              uint16_t color)
{
    int dx = abs((int)x1 - (int)x0);
    int dy = abs((int)y1 - (int)y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;


    while (1)
    {
    	if (x0 < 480-1 && x0 >0 && y0 < 320-1 && y0 >0) {
    		lcd_draw_pixel(x0, y0, color);
    	}
        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err * 2;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}



// MVE-optimized 3D Rotation - processes 4 vertices at once
static void rotate_x_mve(float angle) {
    float32_t cos_a = cosf(angle);
    float32_t sin_a = sinf(angle);

    // Load constants into vector registers
    float32x4_t v_cos = vdupq_n_f32(cos_a);
    float32x4_t v_sin = vdupq_n_f32(sin_a);

    // Process vertices in groups of 4
    for (int i = 0; i < 8; i += 4) {
        // Load y and z coordinates
        float32x4_t v_y = vld1q_f32(&y_coords[i]);
        float32x4_t v_z = vld1q_f32(&z_coords[i]);

        // new_y = y * cos_a - z * sin_a
        float32x4_t new_y = vfmsq_f32(vmulq_f32(v_y, v_cos), v_z, v_sin);

        // new_z = y * sin_a + z * cos_a
        float32x4_t new_z = vfmaq_f32(vmulq_f32(v_z, v_cos), v_y, v_sin);

        // Store results
        vst1q_f32(&y_coords[i], new_y);
        vst1q_f32(&z_coords[i], new_z);
        // x remains unchanged
    }
}

static void rotate_y_mve(float angle) {
    float32_t cos_a = cosf(angle);
    float32_t sin_a = sinf(angle);

    float32x4_t v_cos = vdupq_n_f32(cos_a);
    float32x4_t v_sin = vdupq_n_f32(sin_a);

    for (int i = 0; i < 8; i += 4) {
        float32x4_t v_x = vld1q_f32(&x_coords[i]);
        float32x4_t v_z = vld1q_f32(&z_coords[i]);

        // new_x = x * cos_a + z * sin_a
        float32x4_t new_x = vfmaq_f32(vmulq_f32(v_x, v_cos), v_z, v_sin);

        // new_z = -x * sin_a + z * cos_a
        float32x4_t new_z = vfmsq_f32(vmulq_f32(v_z, v_cos), v_x, v_sin);

        vst1q_f32(&x_coords[i], new_x);
        vst1q_f32(&z_coords[i], new_z);
        // y remains unchanged
    }
}

static void rotate_z_mve(float angle) {
    float32_t cos_a = cosf(angle);
    float32_t sin_a = sinf(angle);

    float32x4_t v_cos = vdupq_n_f32(cos_a);
    float32x4_t v_sin = vdupq_n_f32(sin_a);

    for (int i = 0; i < 8; i += 4) {
        float32x4_t v_x = vld1q_f32(&x_coords[i]);
        float32x4_t v_y = vld1q_f32(&y_coords[i]);

        // new_x = x * cos_a - y * sin_a
        float32x4_t new_x = vfmsq_f32(vmulq_f32(v_x, v_cos), v_y, v_sin);

        // new_y = x * sin_a + y * cos_a
        float32x4_t new_y = vfmaq_f32(vmulq_f32(v_y, v_cos), v_x, v_sin);

        vst1q_f32(&x_coords[i], new_x);
        vst1q_f32(&y_coords[i], new_y);
        // z remains unchanged
    }
}

// MVE-optimized perspective projection
// For division, we'll extract to scalar since MVE doesn't have fdiv
static void project_mve(void) {
    //float32x4_t v_distance = vdupq_n_f32(DISTANCE);
    float32x4_t v_center_x = vdupq_n_f32((float)CENTER_X);
    float32x4_t v_center_y = vdupq_n_f32((float)CENTER_Y);

    // Process 4 vertices at a time
    for (int i = 0; i < 8; i += 4) {
        float32x4_t v_x = vld1q_f32(&x_coords[i]);
        float32x4_t v_y = vld1q_f32(&y_coords[i]);
        float32x4_t v_z = vld1q_f32(&z_coords[i]);

        // Calculate factors for each vertex (scalar division)
        float factors[4];
        factors[0] = DISTANCE / (DISTANCE + vgetq_lane_f32(v_z, 0));
        factors[1] = DISTANCE / (DISTANCE + vgetq_lane_f32(v_z, 1));
        factors[2] = DISTANCE / (DISTANCE + vgetq_lane_f32(v_z, 2));
        factors[3] = DISTANCE / (DISTANCE + vgetq_lane_f32(v_z, 3));

        float32x4_t factor = vld1q_f32(factors);

        // screen_x = CENTER_X + x * factor
        float32x4_t screen_x = vfmaq_f32(v_center_x, v_x, factor);

        // screen_y = CENTER_Y + y * factor
        float32x4_t screen_y = vfmaq_f32(v_center_y, v_y, factor);

        // Convert to int32 and store
        int32x4_t ix = vcvtq_s32_f32(screen_x);
        int32x4_t iy = vcvtq_s32_f32(screen_y);

        // Extract and store
        projected[i].x = (int16_t)vgetq_lane_s32(ix, 0);
        projected[i].y = (int16_t)vgetq_lane_s32(iy, 0);
        projected[i+1].x = (int16_t)vgetq_lane_s32(ix, 1);
        projected[i+1].y = (int16_t)vgetq_lane_s32(iy, 1);
        projected[i+2].x = (int16_t)vgetq_lane_s32(ix, 2);
        projected[i+2].y = (int16_t)vgetq_lane_s32(iy, 2);
        projected[i+3].x = (int16_t)vgetq_lane_s32(ix, 3);
        projected[i+3].y = (int16_t)vgetq_lane_s32(iy, 3);
    }
}

// Main cube update and draw function with MVE
void update_and_draw_cube(uint16_t color) {
    // Scale and copy vertices to separate arrays for SIMD processing
    float32x4_t v_scale = vdupq_n_f32(CUBE_SIZE);

    for (int i = 0; i < 8; i += 4) {
        // Load 4 coordinates manually (can't load from non-contiguous memory)
        float temp_x[4] = {cube_vertices[i].x, cube_vertices[i+1].x,
                           cube_vertices[i+2].x, cube_vertices[i+3].x};
        float temp_y[4] = {cube_vertices[i].y, cube_vertices[i+1].y,
                           cube_vertices[i+2].y, cube_vertices[i+3].y};
        float temp_z[4] = {cube_vertices[i].z, cube_vertices[i+1].z,
                           cube_vertices[i+2].z, cube_vertices[i+3].z};

        float32x4_t vx = vld1q_f32(temp_x);
        float32x4_t vy = vld1q_f32(temp_y);
        float32x4_t vz = vld1q_f32(temp_z);

        // Scale
        vx = vmulq_f32(vx, v_scale);
        vy = vmulq_f32(vy, v_scale);
        vz = vmulq_f32(vz, v_scale);

        // Store to coordinate arrays
        vst1q_f32(&x_coords[i], vx);
        vst1q_f32(&y_coords[i], vy);
        vst1q_f32(&z_coords[i], vz);
    }

    // Apply rotations using MVE
    rotate_x_mve(angle_x);
    rotate_y_mve(angle_y);
    rotate_z_mve(angle_z);

    // Project to 2D using MVE
    project_mve();

    // Draw all edges
    for (int i = 0; i < 12; i++) {
        uint8_t v0 = cube_edges[i][0];
        uint8_t v1 = cube_edges[i][1];

        draw_line(projected[v0].x, projected[v0].y,
                 projected[v1].x, projected[v1].y,
                 color);
    }

    // Update rotation angles for next frame
    angle_x += 0.02f;
    angle_y += 0.03f;
    angle_z += 0.01f;
}

// Alternative: Manual angle control
void set_cube_rotation(float x, float y, float z) {
    angle_x = x;
    angle_y = y;
    angle_z = z;
}

// Get current angles (for debugging)
void get_cube_rotation(float *x, float *y, float *z) {
    if (x) *x = angle_x;
    if (y) *y = angle_y;
    if (z) *z = angle_z;
}

// Reset rotation
void reset_cube_rotation(void) {
    angle_x = 0.0f;
    angle_y = 0.0f;
    angle_z = 0.0f;
}


void run_cube() {
  display_init();
  while(1) {
	  lcd_clear(0, 480, 320);
      update_and_draw_cube(0xFFFF);
  }
}

