/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#ifndef COLOR_COUNT_H_
#define COLOR_COUNT_H_

#include <cstddef>

class ColorCount {
 public:
  size_t value() const { return value_; }
  bool is_zero() const { return value_ == 0; }

  ColorCount();

  size_t operator++(int);

 private:
  size_t value_;
};

#endif
