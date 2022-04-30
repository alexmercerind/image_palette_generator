/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#ifndef IMAGE_PALETTE_GENERATOR_H_
#define IMAGE_PALETTE_GENERATOR_H_

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <memory>
#include <string>
#include <vector>

#include "color.h"

class DLLEXPORT ImagePaletteGenerator {
 public:
  void Open(uint8_t* buffer, int32_t size);

  void Open(FILE* file);

  void Open(std::string file_name);

  void SetQuantized(bool quantized);

  void SetLeftBound(int32_t left_bound);

  void SetRightBound(int32_t right_bound);

  void SetTopBound(int32_t top_bound);

  void SetBottomBound(int32_t bottom_bound);

  Color GetDominantColor();

  std::vector<Color> GetRawPalette(
      int32_t max_color_count = kDefaultPaletteColorCount);

  std::vector<Color> GetPalette(
      int32_t max_color_count = kDefaultPaletteColorCount);

  ~ImagePaletteGenerator();

 private:
  void Rescale();

  std::vector<std::vector<Color>> GetPixels();

  int32_t GetPixelCount();

  Color GetAveragePixelAt(int32_t x, int32_t y, int32_t chunk_dimension);

  Color GetQuantizedColor(Color color);

  static constexpr auto kDefaultPaletteColorCount = 16;
  static constexpr auto kQuantizationWordWidth = 5;
  static constexpr auto kQuantizationChannelWidth = 8;
  static constexpr auto kRescaleWidth = 200;

  bool quantized_ = false;
  uint8_t* data_ = nullptr;
  std::unique_ptr<uint8_t[]> rescaled_data_ = nullptr;
  int32_t width_ = 0;
  int32_t height_ = 0;
  int32_t channels_ = 4;
  int32_t left_bound_ = 0;
  int32_t right_bound_ = INT32_MAX;
  int32_t top_bound_ = 0;
  int32_t bottom_bound_ = INT32_MAX;
  std::vector<std::vector<Color>> pixels_ = {};
  std::vector<Color> palette_ = {};
  std::vector<Color> normalized_palette_ = {};
};

#endif  // IMAGE_PALETTE_GENERATOR_H_
