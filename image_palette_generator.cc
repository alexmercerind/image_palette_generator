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
  palette_.clear();
  tones_.clear();
  Rescale();
}

void ImagePaletteGenerator::Open(FILE* file) {
  stbi_image_free(data_);
  data_ = stbi_load_from_file(file, &width_, &height_, &channels_, 0);
  pixels_.clear();
  palette_.clear();
  tones_.clear();
  Rescale();
}

void ImagePaletteGenerator::Open(char* file_name) {
  stbi_image_free(data_);
  data_ = stbi_load(file_name, &width_, &height_, &channels_, 0);
  pixels_.clear();
  palette_.clear();
  tones_.clear();
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
    width_ = kRescaleWidth;
    height_ = kRescaleWidth * height_ / width_;
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
      auto position = i * stride + j * channels_;
      // RGBA.
      auto color = 0;
      for (auto i = 0; i < channels_; i++) {
        color += (static_cast<int32_t>(data[position + i]) << (i * 8));
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
      auto color = pixels[i][j];
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
  max_color_count += 2;
  auto pixels = GetPixels();
  // No image loaded or pixels found. Return empty |tones_|.
  if (pixels.empty()) {
    return std::vector<Color>{};
  }
  if (!palette_.empty()) {
    return palette_;
  }
  auto pixel_buffer = std::vector<uint8_t>{};
  auto palette_buffer = std::make_unique<uint8_t[]>(max_color_count * 4);
  // Clamp left & top to a minimum of 0.
  auto left = 0 > left_bound_ ? 0 : left_bound_;
  auto top = 0 > top_bound_ ? 0 : top_bound_;
  // Clamp right & bottom to a maximum of the image dimensions.
  auto right = right_bound_ > width_ ? width_ : right_bound_;
  auto bottom = bottom_bound_ > height_ ? height_ : bottom_bound_;
  for (auto i = left; i < right; i++) {
    for (auto j = top; j < bottom; j++) {
      pixel_buffer.emplace_back(pixels[i][j].r());
      pixel_buffer.emplace_back(pixels[i][j].g());
      pixel_buffer.emplace_back(pixels[i][j].b());
      pixel_buffer.emplace_back(pixels[i][j].a());
    }
  }
  exq_feed(static_cast<exq_data*>(exoquant_), pixel_buffer.data(),
           static_cast<int32_t>(pixel_buffer.size() / 4));
  exq_quantize_hq(static_cast<exq_data*>(exoquant_), max_color_count);
  exq_get_palette(static_cast<exq_data*>(exoquant_), palette_buffer.get(),
                  max_color_count);
  for (auto i = 0; i < max_color_count; i++) {
    auto color = Color{palette_buffer[i * 4 + 0], palette_buffer[i * 4 + 1],
                       palette_buffer[i * 4 + 2], palette_buffer[i * 4 + 3]};
    palette_.emplace_back(color);
  }
  exq_free(static_cast<exq_data*>(exoquant_));
  return std::vector<Color>{palette_.begin() + 1, palette_.end() - 1};
}

std::vector<Color> ImagePaletteGenerator::GetTones(int32_t max_color_count) {
  auto chunk_dimension = static_cast<int32_t>(sqrt(max_color_count));
  // Clamp left & top to a minimum of 0.
  auto left = 0 > left_bound_ ? 0 : left_bound_;
  auto top = 0 > top_bound_ ? 0 : top_bound_;
  // Clamp right & bottom to a maximum of the image dimensions.
  auto right = right_bound_ > width_ ? width_ : right_bound_;
  auto bottom = bottom_bound_ > height_ ? height_ : bottom_bound_;
  auto pixels = GetPixels();
  // No image loaded or pixels found. Return empty |tones_|.
  if (pixels.empty()) {
    return std::vector<Color>{};
  }
  // Already calculated.
  if (!tones_.empty()) {
    return tones_;
  }
  for (auto i = left; i < right; i += ((right - left) / chunk_dimension)) {
    for (auto j = top; j < bottom; j += ((bottom - top) / chunk_dimension)) {
      // Browse through a chunk of image & calculate the averaged color.
      //
      // i ---------- i + ((|right| - |left|) / |chunk_dimension|)
      //   |        |
      //   |   <-------------- |averaged_pixel_color|
      //   |        |
      //   ----------
      // j           j + ((|bottom - |top|) / |chunk_dimension|)
      auto averaged_pixel_color = GetAveragePixelAt(i, j, chunk_dimension);
      if (std::find(tones_.begin(), tones_.end(), averaged_pixel_color) ==
          tones_.end()) {
        tones_.emplace_back(averaged_pixel_color);
      }
    }
  }
  std::sort(tones_.begin(), tones_.end(),
            [](Color a, Color b) { return a.Luminance() > b.Luminance(); });
  return tones_;
}

Color ImagePaletteGenerator::GetAveragePixelAt(int32_t x, int32_t y,
                                               int32_t chunk_dimension) {
  // Clamp left & top to a minimum of 0.
  auto left = 0 > left_bound_ ? 0 : left_bound_;
  auto top = 0 > top_bound_ ? 0 : top_bound_;
  // Clamp right & bottom to a maximum of the image dimensions.
  auto right = right_bound_ > width_ ? width_ : right_bound_;
  auto bottom = bottom_bound_ > height_ ? height_ : bottom_bound_;
  auto pixels = GetPixels();
  auto buffer = std::vector<Color>{};
  for (auto i = x; i < std::min(x + ((right - left) / chunk_dimension), right);
       i++) {
    for (auto j = y;
         j < std::min(y + ((bottom - top) / chunk_dimension), bottom); j++) {
      if (pixels_[i][j] != Color{}) {
        buffer.emplace_back(pixels_[i][j]);
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

ImagePaletteGenerator::~ImagePaletteGenerator() { stbi_image_free(data_); }
