/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#include "image_palette_generator.h"

#include <malloc.h>

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

typedef struct _EncodedImage {
  uint8_t* data;
  int32_t width;
  int32_t height;
  int32_t channels;
  int32_t left;
  int32_t right;
  int32_t top;
  int32_t bottom;
} EncodedImage;

int32_t* __image_palette_get_pixels(EncodedImage image) {
  int32_t* pixels = malloc(sizeof(int32_t) * (image.right - image.left) *
                           (image.bottom - image.top));
  // Clamp left & top to a minimum of 0.
  image.left = 0 > image.left ? 0 : image.left;
  image.top = 0 > image.top ? 0 : image.top;
  // Clamp right & bottom to a maximum of the image dimensions.
  image.right = image.right > image.width ? image.width : image.right;
  image.bottom = image.bottom > image.height ? image.height : image.bottom;
  int32_t stride = image.right * image.channels;
  int32_t byte_count = 0;
  for (int32_t row = image.left; row < image.right; row++) {
    for (int32_t col = image.top; col < image.bottom; col++) {
      // Extract image data.
      int32_t position = row * stride + col * image.channels;
      // RGBA.
      int32_t color = 0;
      for (int32_t i = 0; i < image.channels; i++) {
        color += ((int32_t)image.data[position + i] << (i * 8));
      }
      pixels[byte_count++ / image.channels] = color;
      byte_count += image.channels;
    }
  }
  assert(byte_count == (image.right - image.left) * (image.bottom - image.top) *
                           image.channels);
}

ImagePalette image_palette_from_memory(uint8_t* buffer, int32_t size, int left,
                                       int32_t right, int32_t top,
                                       int32_t bottom) {
  ImagePalette palette;
  int32_t width, height, channels;
  uint8_t* data =
      stbi_load_from_memory(buffer, size, &width, &height, &channels, 0);
  EncodedImage image;
  image.data = data;
  image.width = width;
  image.height = height;
  image.channels = channels;
  image.left = left;
  image.right = right;
  image.top = top;
  image.bottom = bottom;
  int32_t* pixels = __image_palette_get_pixels(image);
  return palette;
}

ImagePalette image_palette_from_file(uint8_t* buffer, int32_t size, int left,
                                     int32_t right, int32_t top,
                                     int32_t bottom) {
  ImagePalette palette;
  int32_t width, height, channels;
  uint8_t* data =
      stbi_load_from_memory(buffer, size, &width, &height, &channels, 0);
  EncodedImage image;
  image.data = data;
  image.width = width;
  image.height = height;
  image.channels = channels;
  image.left = left;
  image.right = right;
  image.top = top;
  image.bottom = bottom;
  int32_t* pixels = __image_palette_get_pixels(image);
  return palette;
}

ImagePalette image_palette_from_filepath(uint8_t* buffer, int32_t size,
                                         int left, int32_t right, int32_t top,
                                         int32_t bottom) {
  ImagePalette palette;
  int32_t width, height, channels;
  uint8_t* data =
      stbi_load_from_memory(buffer, size, &width, &height, &channels, 0);
  EncodedImage image;
  image.data = data;
  image.width = width;
  image.height = height;
  image.channels = channels;
  image.left = left;
  image.right = right;
  image.top = top;
  image.bottom = bottom;
  int32_t* pixels = __image_palette_get_pixels(image);
  return palette;
}
