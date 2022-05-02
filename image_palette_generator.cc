/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#include "image_palette_generator.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_map>

#include "color_count.h"

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "exoquant.h"
#ifdef __cplusplus
}
#endif

ImagePaletteGenerator::ImagePaletteGenerator() {
  exoquant_ = exq_init();
  exq_no_transparency(static_cast<exq_data*>(exoquant_));
}

void ImagePaletteGenerator::Open(uint8_t* buffer, int32_t size) {
  stbi_image_free(data_);
  data_ = stbi_load_from_memory(buffer, size, &width_, &height_, &channels_, 0);
  pixels_.clear();
  Rescale();
}

void ImagePaletteGenerator::Open(FILE* file) {
  stbi_image_free(data_);
  data_ = stbi_load_from_file(file, &width_, &height_, &channels_, 0);
  pixels_.clear();
  Rescale();
}

void ImagePaletteGenerator::Open(char* file_name) {
  stbi_image_free(data_);
  data_ = stbi_load(file_name, &width_, &height_, &channels_, 0);
  pixels_.clear();
  Rescale();
}

void ImagePaletteGenerator::SetQuantized(bool quantized) {
  quantized_ = quantized;
}

void ImagePaletteGenerator::SetLeftBound(int32_t left_bound) {
  left_bound_ = left_bound;
}

void ImagePaletteGenerator::SetRightBound(int32_t right_bound) {
  right_bound_ = right_bound;
}

void ImagePaletteGenerator::SetTopBound(int32_t top_bound) {
  top_bound_ = top_bound;
}

void ImagePaletteGenerator::SetBottomBound(int32_t bottom_bound) {
  bottom_bound_ = bottom_bound;
}

void ImagePaletteGenerator::Rescale() {
  // Downscale the image dimensions to reduce operations & number of elements in
  // the |pixels_|.
  if (width_ > kRescaleWidth) {
    rescaled_data_ = std::make_unique<uint8_t[]>(static_cast<size_t>(
        pow(kRescaleWidth, 2) * height_ / width_ * channels_));
    stbir_resize_uint8(data_, width_, height_, 0, rescaled_data_.get(),
                       kRescaleWidth, kRescaleWidth * height_ / width_, 0,
                       channels_);
    auto original_width = width_;
    auto original_height = height_;
    width_ = kRescaleWidth;
    height_ = kRescaleWidth * original_height / original_width;
  } else {
    rescaled_data_ = nullptr;
  }
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

std::vector<std::vector<Color>> ImagePaletteGenerator::GetPixels() {
  uint8_t* data = rescaled_data_ ? rescaled_data_.get() : data_;
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
    pixels_.emplace_back(std::vector<Color>{});
    for (auto j = top; j < bottom; j++) {
      // Extract image data.
      auto position = j * stride + i * channels_;
      // RGBA.
      auto color = 0;
      for (auto j = 0; j < channels_; j++) {
        color += (static_cast<int32_t>(data[position + j]) << (j * 8));
      }
      // Image without alpha channel. Explicitly add 0xFF.
      if (channels_ == 3) {
        color += (0xFF << 24);
      }
      pixels_.back().emplace_back(quantized_ ? GetQuantizedColor(Color(color))
                                             : Color(color));
      byte_count += channels_;
    }
  }
  assert(byte_count == GetPixelCount() * channels_);
  return pixels_;
}

Color ImagePaletteGenerator::GetDominantColor() {
  auto pixels = GetPixels();
  if (pixels.empty()) {
    return Color{};
  }
  size_t dominant_count = 0;
  auto dominant_color = Color{};
  auto color_map = std::unordered_map<Color, ColorCount>();
  for (auto i = 0; i < pixels.size(); i++) {
    for (auto j = 0; j < pixels[i].size(); j++) {
      auto color = pixels[j][i];
      if (quantized_) {
        color = GetQuantizedColor(color);
      }
      color_map[color]++;
    }
  }
  for (auto& [color, count] : color_map) {
    if (count.value() > dominant_count) {
      dominant_count = count.value();
      dominant_color = color;
    }
  }
  return dominant_color;
}

std::vector<Color> ImagePaletteGenerator::GetPalette(int32_t max_color_count) {
  auto palette = std::vector<Color>{};
  auto pixels = GetPixels();
  if (pixels.empty()) {
    return std::vector<Color>{};
  }
  auto pixel_buffer = std::vector<uint8_t>{};
  pixel_buffer.reserve(pixels.size() * pixels[0].size() * 4);
  auto palette_buffer = std::make_unique<uint8_t[]>(max_color_count * 4);
  for (auto i = 0; i < pixels[0].size(); i++) {
    for (auto j = 0; j < pixels.size(); j++) {
      pixel_buffer.emplace_back(pixels[j][i].r());
      pixel_buffer.emplace_back(pixels[j][i].g());
      pixel_buffer.emplace_back(pixels[j][i].b());
      pixel_buffer.emplace_back(pixels[j][i].a());
    }
  }
  exq_feed(static_cast<exq_data*>(exoquant_), pixel_buffer.data(),
           static_cast<int32_t>(width_ * height_));
  exq_quantize_hq(static_cast<exq_data*>(exoquant_), max_color_count);
  exq_get_palette(static_cast<exq_data*>(exoquant_), palette_buffer.get(),
                  max_color_count);
  for (auto i = 0; i < max_color_count; i++) {
    auto color = Color{palette_buffer[i * 4 + 0], palette_buffer[i * 4 + 1],
                       palette_buffer[i * 4 + 2], palette_buffer[i * 4 + 3]};
    palette.emplace_back(color);
  }
  return std::vector<Color>{palette.begin(), palette.end()};
}

std::vector<Color> ImagePaletteGenerator::GetTones(int32_t max_color_count) {
  auto tones = std::vector<Color>{};
  auto chunk_dimension = static_cast<int32_t>(sqrt(max_color_count));
  auto pixels = GetPixels();
  // No image loaded or pixels found. Return empty |tones|.
  if (pixels.empty()) {
    return std::vector<Color>{};
  }
  auto count = 0;
  for (auto i = 0; i < pixels[0].size();
       i += pixels[0].size() / chunk_dimension) {
    for (auto j = 0; j < pixels.size(); j += pixels.size() / chunk_dimension) {
      if (count > max_color_count) {
        return tones;
      }
      auto averaged_pixel_color = GetAveragePixelAt(i, j, chunk_dimension);
      if (std::find(tones.begin(), tones.end(), averaged_pixel_color) ==
          tones.end()) {
        tones.emplace_back(averaged_pixel_color);
        count++;
      }
    }
  }
  return tones;
}

Color ImagePaletteGenerator::GetAveragePixelAt(int32_t x, int32_t y,
                                               int32_t chunk_dimension) {
  auto pixels = GetPixels();
  auto buffer = std::vector<Color>{};
  for (auto i = std::max(x, 0);
       i < std::min(x + (pixels[0].size() / chunk_dimension), pixels[0].size());
       i++) {
    for (auto j = std::max(y, 0);
         j < std::min(y + (pixels.size() / chunk_dimension), pixels.size());
         j++) {
      if (pixels[j][i] != Color{}) {
        buffer.emplace_back(pixels[j][i]);
      }
    }
  }
  size_t r = 0, g = 0, b = 0, a = 0;
  for (auto& color : buffer) {
    r += color.r();
    g += color.g();
    b += color.b();
    a += color.a();
  }
  if (buffer.empty()) {
    return Color{};
  }
  auto color = Color{static_cast<uint8_t>(r / buffer.size()),
                     static_cast<uint8_t>(g / buffer.size()),
                     static_cast<uint8_t>(b / buffer.size()),
                     static_cast<uint8_t>(a / buffer.size())};
  return quantized_ ? GetQuantizedColor(color) : color;
}

Color ImagePaletteGenerator::GetQuantizedColor(Color color) {
  auto word_mask = ((1 << kQuantizationWordWidth) - 1)
                   << (kQuantizationChannelWidth - kQuantizationWordWidth);
  return Color{static_cast<uint8_t>(color.r() & word_mask),
               static_cast<uint8_t>(color.g() & word_mask),
               static_cast<uint8_t>(color.b() & word_mask), color.a()};
}

ImagePaletteGenerator::~ImagePaletteGenerator() {
  stbi_image_free(data_);
  exq_free(static_cast<exq_data*>(exoquant_));
}
