/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#ifndef IMAGE_PALETTE_GENERATOR_H_
#define IMAGE_PALETTE_GENERATOR_H_

#include <memory>
#include <string>
#include <vector>

#include "color.h"

class ImagePaletteGenerator {
 public:
  void Open(uint8_t* buffer, int32_t size, bool rescale = true);

  void Open(FILE* file, bool rescale = true);

  void Open(std::string file_name, bool rescale = true);

  void SetQuantized(bool quantized);

  void SetLeftBound(int left_bound);

  void SetRightBound(int right_bound);

  void SetTopBound(int top_bound);

  void SetBottomBound(int bottom_bound);

  Color GetDominantColor();

  ~ImagePaletteGenerator();

 private:
  void Rescale();

  std::vector<Color> GetPixels();

  int32_t GetPixelCount();

  Color GetQuantizedColor(Color color);

  static constexpr auto kQuantizedWordWidth = 5;
  static constexpr auto kQuantizedChannelWidth = 8;

  // TODO: Make |kRescaleWidth| configurable from client code.
  static constexpr auto kRescaleWidth = 400;

  bool quantized_ = true;
  uint8_t* data_ = nullptr;
  std::unique_ptr<uint8_t[]> rescaled_data_ = nullptr;
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
