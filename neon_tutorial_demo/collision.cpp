#include <stdio.h>
#include <arm_neon.h>

struct circle // 12B
{
  float radius;
  float x;
  float y;
};

bool does_collide(circle& c1, circle& c2)
{
  // Two circles collide if the distance from c1 to c2 is less
  // than the sum of their radii, or equivalently if the squared
  // distance is less than the square of the sum of the radii.
  float delta_x = c1.x - c2.x;
  float delta_y = c1.y - c2.y;
  float deltas_squared = (delta_x * delta_x) + (delta_y * delta_y);
  float radius_sum_squared = (c1.radius + c2.radius) * (c1.radius + c2.radius);
  return deltas_squared <= radius_sum_squared;
}

struct circle_aligned
{
  float x;
  float y;
  float radius;
} __attribute__((aligned (64)));
// 我们要按照一块来读取这个变量，所以需要内存对齐，理论上讲这里 16字节对齐就够了
// 如果不对齐，那么访存次数会变多

bool does_collide_neon(circle_aligned const& c1, circle_aligned const& c2)
{
  float32x2_t c1_coords = vld1_f32(&c1.x); // load c1.x and c1.y to registers
  float32x2_t c2_coords = vld1_f32(&c2.x); // load c2.x and c2.y to registers

  float32x2_t deltas = vsub_f32(c1_coords, c2_coords); // c1.y - c2.y and c1.x - c2.x
  // cal the (c1.y - c2.y)^2 and (c1.x - c2.x)^2
  float32x2_t deltas_squared = vmul_f32(deltas, deltas); 

  // vpadds_f32 adds the two floating-point vector elements to give a single scalar.
  float sum_deltas_squared = vpadds_f32(deltas_squared);

  float radius_sum = c1.radius + c2.radius;
  float radius_sum_squared = radius_sum * radius_sum;
  return sum_deltas_squared <= radius_sum_squared;
}

int main()
{
  circle c1;
  c1.radius = 2.0;
  c1.x = 2.0;
  c1.y = 4.0;

  circle c2;
  c2.radius = 1.0;
  c2.x = 6.0;
  c2.y = 1.0;

  if (does_collide(c1, c2)) {
    printf("Circles collide\n");
  } else {
    printf("Circles do not collide\n");
  }

  circle_aligned c3;
  c3.radius = 2.0;
  c3.x = 2.0;
  c3.y = 4.0;

  circle_aligned c4;
  c4.radius = 1.0;
  c4.x = 6.0;
  c4.y = 1.0;
  if (does_collide_neon(c3, c4)) {
    printf("Circles collide\n");
  } else {
    printf("Circles do not collide\n");
  }
  return (0);
}
