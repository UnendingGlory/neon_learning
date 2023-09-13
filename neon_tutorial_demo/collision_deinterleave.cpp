#include <arm_neon.h>
#include <stdio.h>


// Structure containing data for a single collider circle
struct circle
{
  float x;
  float y;
  float radius;
};


// Structure containing an array of pointers to data for **multiple circles**
struct circles
{
  size_t size;
  float* xs;
  float* ys;
  float* radii;
};


void does_collide_neon_deinterleaved(circles const& input, circle& collider, bool* out)
{
  // Duplicate the collider properties in 3 separate 4-lane vector registers
  float32x4_t c1_x = vdupq_n_f32(collider.x); // use 128 bits registers
  float32x4_t c1_y = vdupq_n_f32(collider.y);
  float32x4_t c1_r = vdupq_n_f32(collider.radius);

  for (size_t offset = 0; offset != input.size; offset += 4)
  {
    // Perform 4 collision tests at a time
    float32x4_t x = vld1q_f32(input.xs + offset);
    float32x4_t y = vld1q_f32(input.ys + offset);

    float32x4_t delta_x = vsubq_f32(c1_x, x);
    float32x4_t delta_y = vsubq_f32(c1_y, y);
    float32x4_t delta_x_squared = vmulq_f32(delta_x, delta_x);
    float32x4_t delta_y_squared = vmulq_f32(delta_y, delta_y);
    float32x4_t sum_deltas_squared = vaddq_f32(delta_x_squared, delta_y_squared);

    float32x4_t r = vld1q_f32(input.radii + offset);
    float32x4_t radius_sum = vaddq_f32(c1_r, r);
    float32x4_t radius_sum_squared = vmulq_f32(radius_sum, radius_sum);
    uint32x4_t mask = vcltq_f32(sum_deltas_squared, radius_sum_squared);

    // Unpack the results in each lane
    out[offset] = 1 & vgetq_lane_u32(mask, 0);
    out[offset + 1] = 1 & vgetq_lane_u32(mask, 1);
    out[offset + 2] = 1 & vgetq_lane_u32(mask, 2);
    out[offset + 3] = 1 & vgetq_lane_u32(mask, 3);
  }
}


int main()
{
  int num_input = 4;
  float input_x[num_input] __attribute__((aligned (64)));
  float input_y[num_input] __attribute__((aligned (64)));
  float input_r[num_input] __attribute__((aligned (64)));
  bool output[num_input]   __attribute__((aligned (64)));

  // Set up the data for multiple circles
  for (int i = 0; i < num_input; i++) {
    input_x[i] = i*2;
    input_y[i] = i*3;
    input_r[i] = i;
    output[i] = 0;
  }

  // Create input object containing pointers to array data for multiple circles
  circles c1;
  c1.size = num_input;
  c1.radii = input_r;
  c1.xs = input_x;
  c1.ys = input_y;

  // Create collider object containing data for a single circle
  circle c2;
  c2.radius = 5.0;
  c2.x = 10.0;
  c2.y = 10.0;

  // Test whether the collider circle collides with any of the input circles, returning results in output
  does_collide_neon_deinterleaved(c1, c2, output);

  // Iterate over the returned output data and display results
  for (int i = 0; i < num_input; i++) {
    if (output[i]) {
      printf("Circle %d at (%.1f, %.1f) with radius %.1f collides\n", i, input_x[i], input_y[i], input_r[i]);
    } else {
    printf("Circle %d at (%.1f, %.1f) with radius %.1f does not collide\n", i, input_x[i], input_y[i], input_r[i]);
    }
  }
  return (0);
}
