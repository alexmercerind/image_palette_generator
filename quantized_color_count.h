/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#ifndef QUANTIZED_COLOR_COUNT_H_
#define QUANTIZED_COLOR_COUNT_H_

#include <stdint.h>

class QuantizedColorCount {
 public:
  int32_t value() { return value_; }

  QuantizedColorCount() : value_(0) {}

  int operator++(int) { return value_++; }

 private:
  int32_t value_;
};

#endif
