/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#ifndef IMAGE_PALETTE_GENERATOR_H_
#define IMAGE_PALETTE_GENERATOR_H_

#include <string>
#include <vector>

#include "color.h"

class ImagePaletteGenerator {
 public:
  void Open(uint8_t* buffer, int32_t size);

  void Open(FILE* file);

  void Open(std::string file_name);

  void SetQuantized(bool quantized);

  void SetLeftBound(int left_bound);

  void SetRightBound(int right_bound);

  void SetTopBound(int top_bound);

  void SetBottomBound(int bottom_bound);

  std::vector<Color> GetPixels();

  Color GetDominantColor();

  int32_t GetPixelCount();

  Color GetQuantizedColor(Color color);

 private:
  static constexpr auto kQuantizedWordWidth = 5;
  static constexpr auto kQuantizedChannelWidth = 8;

  bool quantized_ = true;
  uint8_t* data_ = nullptr;
  int32_t width_ = 0;
  int32_t height_ = 0;
  int32_t channels_ = 4;
  int32_t left_bound_ = 0;
  int32_t right_bound_ = INT32_MAX;
  int32_t top_bound_ = 0;
  int32_t bottom_bound_ = INT32_MAX;
  std::vector<Color> pixels_ = {};
};

#endif  // IMAGE_PALETTE_GENERATOR_H_
