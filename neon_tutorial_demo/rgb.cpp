#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arm_neon.h>
// #include "NEONvsSSE.h"

// if len_color is 15, then rgb should hold 15 * 3 = 45 uint8_t numbers
void rgb_deinterleave_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color) {
    /*
     * Take the elements of "rgb" and store the individual colors "r", "g", and "b".
     */
    for (int i = 0; i < len_color; i++) {
        r[i] = rgb[3*i];
        g[i] = rgb[3*i + 1];
        b[i] = rgb[3*i + 2];
    }
}

/**
 * @brief remember that here is the SIMD programming pattern
 * r1  r2  r3
 * R0  G0  B0 
 * R1  G1  B1
 * ...
 * R15 G15 B15
 */
void rgb_deinterleave_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color) {
    /*
     * Take the elements of "rgb" and store the individual colors "r", "g", and "b"
     */
    int num8x16 = len_color / 16;
    // a struct of which stores three 128 bits registers, 
    // here 3 registers load numbers simultaneously
    uint8x16x3_t intlv_rgb; 
    // here each neon register holds 128 bits
    for (int i = 0; i < num8x16; i++) {
        intlv_rgb = vld3q_u8(rgb + 3*16*i); // load 16 pixels which contains 16 * 3 RGB
        // here intlv_rgb.val[0] stores {R0, R1, R2 ... R15}
        // store 16 nums into r just one operation
        vst1q_u8(r + 16*i, intlv_rgb.val[0]); 
        vst1q_u8(g + 16*i, intlv_rgb.val[1]);
        vst1q_u8(b + 16*i, intlv_rgb.val[2]);
    }
}

// aarch64-linux-android24-clang++ rgb.cpp -o rgb
int main() {
    const int pixels = 32 * 32; // can not be too large here
    uint8_t rgb[pixels];
    for (int i = 0; i < pixels; ++i) {
        rgb[i] = rand() % 256;
    }
    uint8_t r[pixels], g[pixels], b[pixels];

    clock_t start, end;
    double elapsed;
    
    start = clock();
    rgb_deinterleave_c(r, g, b, rgb, pixels);
    end = clock();
    elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("c time: %f s\n", elapsed);
    
    start = clock();
    rgb_deinterleave_neon(r, g, b, rgb, pixels);
    end = clock();
    elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("c time: %f s\n", elapsed);
    return 0;
}
