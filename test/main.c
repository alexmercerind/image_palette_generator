#include <stdint.h>
#include <stdio.h>

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

int32_t main() {
  int32_t row_start = 0;
  int32_t col_start = 0;
  int32_t row_end, col_end, n;
  uint8_t* data = stbi_load("image.png", &row_end, &col_end, &n, 0);
  printf("%d %d %d\n", row_end, col_end, n);
  int32_t stride = row_end * n;
  int32_t byte_count = 0;
  for (int32_t row = row_start; row < row_end; row++) {
    for (int32_t col = col_start; col < col_end; col++) {
      int32_t position = row * stride + col * n;
      printf("rgba(");
      for (int32_t i = 0; i < n - 1; i++) {
        printf("%d, ", data[position + i]);
      }
      printf("%d", data[position + n - 1]);
      printf(")\n");
      byte_count += n;
    }
  }
  printf("%d\n", byte_count);
  return 0;
}
