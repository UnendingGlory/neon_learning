#include "boxFilter.h"
#include <chrono>
#include <random>
#include <iostream>

#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

// naive algorithm, O(height * width * (2radius+1) * (2radius+1))
void BoxFilter::filter(float *input, int radius, int height, int width, float *output) {
  for (int h = 0; h < height; ++h) {
    int height_sift = h * width;
    for (int w = 0; w < width; ++w) {
      int start_h = std::max(0, h - radius); // boundary 
      int end_h = std::min(height - 1, h + radius);
      int start_w = std::max(0, w - radius);
      int end_w = std::min(width - 1, w + radius);

      float tmp = 0;
      for (int sh = start_h; sh <= end_h; ++sh) {
        for (int sw = start_w; sw <= end_w; ++ sw) {
          tmp += input[sh * width + sw];
        }
      }

      output[height_sift + w] = tmp;
    }
  }
}

// split row and col, O(height * width * (2radius+1))
void BoxFilter::fastFilter(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;
    for (int w = 0; w < width; ++w) {
      int start_w = std::max(0, w - radius);
      int end_w = std::min(width - 1, w + radius);

      float tmp = 0;
      for (int sw = start_w; sw <= end_w; ++ sw) {
        tmp += input[shift + sw];
      }

      cachePtr[shift + w] = tmp; // cache each point row sum
    }
  }

  // sum vertical
  for (int h = 0; h < height; ++h) {
    int out_shift = h * width;
    int start_h = std::max(0, h - radius);
    int end_h = std::min(height - 1, h + radius);

    for (int sh = start_h; sh <= end_h; ++sh) {
      int shift = sh * width;
      for (int w = 0; w < width; ++w) {
        output[out_shift + w] += cachePtr[shift + w];
      }
    }
  }
}

// sliding window based on fastFilter, O(height * width)
void BoxFilter::fastFilterV1(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizontal
  for (int h = 0; h < height; ++h)  {
    int shift = h * width;
    
    // presum of the left radius
    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    // head of the row
    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius]; // end point of the box row
      cachePtr[shift + w] = tmp;
    }
    
    // middle of the row
    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    // end of the row
    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    // sum vertically, the same as horizontally
    for (int w = 0; w < width; ++w) {
      float tmp = 0;
      for (int h = 0; h < radius; ++h) {
        tmp += cachePtr[h * width + w];
      }

      for (int h = 0; h <= radius; ++h) {
        tmp += cachePtr[(h + radius) * width + w];
        output[h * width + w] = tmp;
      }

      for (int h = radius + 1; h <= height - 1 - radius; ++h) {
        tmp += cachePtr[(h + radius) * width + w];
        tmp -= cachePtr[(h - radius - 1) * width + w];
        output[h * width + w] = tmp;
      }

      for (int h = height - radius; h < height; ++h) {
        tmp -= cachePtr[(h - radius - 1) * width + w];
        output[h * width + w] = tmp;
      }
    }
  }
}

// rearrange loop to reduce cache miss based on V1, O(height * width)
void BoxFilter::fastFilterV2(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizontal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  // need an array to store the sum of the col
  float *colSumPtr = &(colSum[0]);
  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  }
  // sum vertical, presum of the top height
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] += cachePtr[shift + w];
    }
  }

  for (int h = 0; h <= radius; ++h) {
    // pre calculate local variable out of the loop
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] += addPtr[w];
      outPtr[w] = colSumPtr[w];
    }
  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift;
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] += addPtr[w];
      colSumPtr[w] -= subPtr[w];
      outPtr[w] = colSumPtr[w];
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] -= subPtr[w];
      outPtr[w] = colSumPtr[w];
    }
  }
}

// neon intrinsics to accelerate the column sum
void BoxFilter::fastFilterV2NeonIntrinsics(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  // here can not use simd, because tmp has dependencies
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  int n = width >> 2;
  int re = width - (n << 2); // remain
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }

  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);
      _tmp = vsubq_f32(_tmp, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}

// careful to use assembly, it's very easy to write negative optimization
void BoxFilter::fastFilterV2NeonAsm(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  int n = width >> 2;
  int re = width - (n << 2);
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }

  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
#if __aarch64__ // armv8
    asm volatile(                    // volatile let compiler not do optimization for the assembly
      // Assembler Template
      "0:                           \n"
      "ld1  {v0.4s}, [%0], #16      \n" // _add, tmpAddPtr += 4
      "ld1  {v1.4s}, [%1], #16      \n" // _sub, tmpSubPtr += 4
      "ld1  {v2.4s}, [%2]           \n" // _colSum
      "fadd v3.4s, v0.4s, v2.4s     \n" // _tmp
      "fsub v4.4s, v3.4s, v1.4s     \n" // _tmp
      "st1  {v4.4s}, [%3], #16      \n" // store to the tmpOutPtr memory address and tmpOutPtr += 4
      "st1  {v4.4s}, [%2], #16      \n" // tmpColSumPtr += 4
      "subs %4, %4, #1              \n" // nn--
      "bne  0b                      \n" // stop when nn = 0
      // OutputOperands
      : "=r"(tmpAddPtr),
      "=r"(tmpSubPtr),
      "=r"(tmpColSumPtr),
      "=r"(tmpOutPtr),
      "=r"(nn)
      // InputOperands
      : "0"(tmpAddPtr),
      "1"(tmpSubPtr),
      "2"(tmpColSumPtr),
      "3"(tmpOutPtr),
      "4"(nn)
      // Clobbers
      : "cc", "memory", "v0", "v1", "v2", "v3", "v4"
    );
#endif
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}

// use prefetch instruction to preload to l1 cache before the storing instruction, 
// to avoid directly transfer between memory and register
void BoxFilter::fastFilterV2NeonAsmV1(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  int n = width >> 2;
  int re = width - (n << 2);
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }
  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
#if __aarch64__ // armv8
    asm volatile(                    
      // Assembler Template
      "0:                           \n"
      "prfm pldl1keep, [%0, #64]    \n" // prefetch to l1 cache
      "ld1  {v0.4s}, [%0], #16      \n"
      "prfm pldl1keep, [%1, #64]    \n"
      "ld1  {v1.4s}, [%1], #16      \n"
      "prfm pldl1keep, [%2, #64]    \n"
      "ld1  {v2.4s}, [%2]           \n"
      "fadd v3.4s, v0.4s, v2.4s     \n"
      "fsub v4.4s, v3.4s, v1.4s     \n"
      "st1  {v4.4s}, [%3], #16      \n"
      "st1  {v4.4s}, [%2], #16      \n"
      "subs %4, %4, #1              \n"
      "bne  0b                      \n"
      // OutputOperands
      : "=r"(tmpAddPtr),
      "=r"(tmpSubPtr),
      "=r"(tmpColSumPtr),
      "=r"(tmpOutPtr),
      "=r"(nn)
      // InputOperands
      : "0"(tmpAddPtr),
      "1"(tmpSubPtr),
      "2"(tmpColSumPtr),
      "3"(tmpOutPtr),
      "4"(nn)
      // Clobbers
      : "cc", "memory", "v0", "v1", "v2", "v3", "v4"
    );
#endif
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}


// Dual-Issue Pipeline (and instruction reordering)
void BoxFilter::fastFilterV2NeonAsmV2(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  int n = width >> 2;
  int re = width - (n << 2);
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }
  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = width >> 3; // double instructions at the same time
    int remain = re - (nn << 3);
#if __ARM_NEON
#if __aarch64__ // armv8
    asm volatile(                    
      // Assembler Template
      "0:                                   \n"
      "prfm pldl1keep, [%0, #64]            \n" // prefetch to l1 cache
      "ld1  {v0.4s, v1.4s}, [%0], #32       \n"

      // this following two lines can be put behind fadd
      "prfm pldl1keep, [%1, #64]           \n"
      "ld1  {v2.4s, v3.4s}, [%1], #32       \n"

      "prfm pldl1keep, [%2, #64]           \n"
      "ld1  {v4.4s, v5.4s}, [%2]            \n"

      "fadd v6.4s, v0.4s, v4.4s             \n"
      "fadd v7.4s, v1.4s, v5.4s             \n"

      // "prfm pldl1keep, [%1, #64]           \n"
      // "ld1  {v2.4s, v3.4s}, [%1], #32       \n"

      "fsub v8.4s, v6.4s, v2.4s             \n"
      "fsub v9.4s, v7.4s, v3.4s             \n"

      "st1  {v8.4s, v9.4s}, [%3], #32       \n"
      "st1  {v8.4s, v9.4s}, [%2], #32       \n"
      "subs %4, %4, #1                      \n"
      "bne  0b                              \n"
      // OutputOperands
      : "=r"(tmpAddPtr),
      "=r"(tmpSubPtr),
      "=r"(tmpColSumPtr),
      "=r"(tmpOutPtr),
      "=r"(nn)
      // InputOperands
      : "0"(tmpAddPtr),
      "1"(tmpSubPtr),
      "2"(tmpColSumPtr),
      "3"(tmpOutPtr),
      "4"(nn)
      // Clobbers
      : "cc", "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9"
    );
#endif
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}
