/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#include <stdint.h>
#include <stdio.h>

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

typedef struct _RGBA {
  int8_t r;
  int8_t g;
  int8_t b;
  int8_t a;
} RGBA;

typedef struct _ImagePalette {
  RGBA colors[IMAGE_PALETTE_GENERATOR_COLORS_MAX];
  int32_t count;
} ImagePalette;

DLLEXPORT RGBA Int32ToRGBA(int32_t color);

DLLEXPORT int32_t RGBAToInt32(RGBA color);

DLLEXPORT char* RGBAToCStr(RGBA color);

DLLEXPORT ImagePalette ImagePaletteFromMemory(uint8_t* buffer, int32_t size,
                                              int32_t left, int32_t right,
                                              int32_t top, int32_t bottom);

DLLEXPORT ImagePalette ImagePaletteFromFile(FILE* file, int32_t left,
                                            int32_t right, int32_t top,
                                            int32_t bottom);

DLLEXPORT ImagePalette ImagePaletteFromFileName(char* file_name, int32_t left,
                                                int32_t right, int32_t top,
                                                int32_t bottom);

#ifdef __cplusplus
}
#endif

#endif  // IMAGE_PALETTE_GENERATOR_H_
