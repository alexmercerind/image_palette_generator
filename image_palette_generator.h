#include <stdint.h>

#ifndef IMAGE_PALETTE_GENERATOR_H_
#define IMAGE_PALETTE_GENERATOR_H_

#define IMAGE_PALETTE_GENERATOR_COLORS_MAX 20

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ImagePalette {
  int32_t colors[IMAGE_PALETTE_GENERATOR_COLORS_MAX];
  int32_t count;
} ImagePalette;

DLLEXPORT ImagePalette image_palette_from_memory(uint8_t* data, int32_t size,
                                                 int left, int32_t top,
                                                 int32_t right, int32_t bottom);

#ifdef __cplusplus
}
#endif

#endif  // IMAGE_PALETTE_GENERATOR_H_
