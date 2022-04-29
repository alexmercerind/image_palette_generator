/// This file is a part of image_palette_generator
/// (https://github.com/alexmercerind/image_palette_generator).
///
/// Copyright (c) 2022, Hitesh Kumar Saini <saini123hitesh@gmail.com>.
/// All rights reserved.
/// Use of this source code is governed by MIT license that can be found in the
/// LICENSE file.

#ifndef COLOR_H_
#define COLOR_H_

#include <string>

class Color {
 public:
  uint8_t r() const { return r_; };
  uint8_t g() const { return g_; };
  uint8_t b() const { return b_; };
  uint8_t a() const { return a_; };

  bool operator==(const Color color) const {
    return r_ == color.r_ && g_ == color.g_ && b_ == color.b_ && a_ == color.a_;
  }

  bool operator!=(const Color color) const { return !(*this == color); }

  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

  Color(int32_t color);

  Color();

  int32_t ToInt32();

  std::string ToString();

  bool IsDark();

  bool IsLight();

 private:
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
  uint8_t a_;
};

namespace std {

template <>
struct hash<Color> {
  size_t operator()(Color const& color) const {
    return (color.r() << 0) | (color.g() << 8) | (color.b() << 16) |
           (color.a() << 24);
  }
};

}  // namespace std

#endif  // COLOR_H_
