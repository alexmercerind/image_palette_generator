/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#include "color.h"

#include <sstream>

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r_(r), g_(g), b_(b), a_(a) {}

Color::Color(int32_t color) {
  r_ = (color >> 0) & 0xFF;
  g_ = (color >> 8) & 0xFF;
  b_ = (color >> 16) & 0xFF;
  a_ = (color >> 24) & 0xFF;
}

Color::Color() : r_(0), g_(0), b_(0), a_(0){};

void Color::TakeAverage(Color color) {
  r_ = (r_ + color.r()) / 2;
  g_ = (g_ + color.g()) / 2;
  b_ = (b_ + color.b()) / 2;
  a_ = (a_ + color.a()) / 2;
}

int32_t Color::ToInt32() {
  return (r_ << 0) | (g_ << 8) | (b_ << 16) | (a_ << 24);
}

std::string Color::ToString() {
  std::stringstream stream;
  stream << "rgba(" << static_cast<int32_t>(r_) << ", "
         << static_cast<int32_t>(g_) << ", " << static_cast<int32_t>(b_) << ", "
         << static_cast<int32_t>(a_) << ")";
  return stream.str();
}
