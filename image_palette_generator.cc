/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#include "image_palette_generator.h"

#include <memory>
#include <unordered_map>

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION

#include "external/stb/stb_image.h"

void ImagePaletteGenerator::Open(uint8_t* buffer, int32_t size) {
  data_ = stbi_load_from_memory(buffer, size, &width_, &height_, &channels_, 0);
  pixels_.clear();
}

void ImagePaletteGenerator::Open(FILE* file) {
  data_ = stbi_load_from_file(file, &width_, &height_, &channels_, 0);
  pixels_.clear();
}

void ImagePaletteGenerator::Open(std::string file_name) {
  data_ = stbi_load(file_name.c_str(), &width_, &height_, &channels_, 0);
  pixels_.clear();
}

void ImagePaletteGenerator::SetQuantized(bool quantized) {
  quantized_ = quantized;
}

void ImagePaletteGenerator::SetLeftBound(int left_bound) {
  left_bound_ = left_bound;
}

void ImagePaletteGenerator::SetRightBound(int right_bound) {
  right_bound_ = right_bound;
}

void ImagePaletteGenerator::SetTopBound(int top_bound) {
  top_bound_ = top_bound;
}

void ImagePaletteGenerator::SetBottomBound(int bottom_bound) {
  bottom_bound_ = bottom_bound;
}

int32_t ImagePaletteGenerator::GetPixelCount() {
  // Clamp left & top to a minimum of 0.
  auto left = 0 > left_bound_ ? 0 : left_bound_;
  auto top = 0 > top_bound_ ? 0 : top_bound_;
  // Clamp right & bottom to a maximum of the image dimensions.
  auto right = right_bound_ > width_ ? width_ : right_bound_;
  auto bottom = bottom_bound_ > height_ ? height_ : bottom_bound_;
  return (right - left) * (bottom - top);
}

std::vector<Color> ImagePaletteGenerator::GetPixels() {
  if (!pixels_.empty()) {
    return pixels_;
  }
  // Clamp left & top to a minimum of 0.
  auto left = 0 > left_bound_ ? 0 : left_bound_;
  auto top = 0 > top_bound_ ? 0 : top_bound_;
  // Clamp right & bottom to a maximum of the image dimensions.
  auto right = right_bound_ > width_ ? width_ : right_bound_;
  auto bottom = bottom_bound_ > height_ ? height_ : bottom_bound_;
  auto stride = (right - left) * channels_;
  auto byte_count = 0;
  for (auto i = left; i < right; i++) {
    for (auto j = top; j < bottom; j++) {
      // Extract image data.
      auto position = i * stride + j * channels_;
      // RGBA.
      auto color = 0;
      for (auto i = 0; i < channels_; i++) {
        color += ((int32_t)data_[position + i] << (i * 8));
      }
      // Image without alpha channel. Explicitly add 0xFF.
      if (channels_ == 3) {
        color += (0xFF << 24);
      }
      pixels_.emplace_back(quantized_ ? GetQuantizedColor(Color(color))
                                      : Color(color));
      byte_count += channels_;
    }
  }
  assert(byte_count == GetPixelCount() * channels_);
  return pixels_;
}

Color ImagePaletteGenerator::GetQuantizedColor(Color color) {
  auto word_mask = ((1 << kQuantizedWordWidth) - 1)
                   << (kQuantizedChannelWidth - kQuantizedWordWidth);
  return Color{static_cast<uint8_t>(color.r() & word_mask),
               static_cast<uint8_t>(color.g() & word_mask),
               static_cast<uint8_t>(color.b() & word_mask), color.a()};
}
