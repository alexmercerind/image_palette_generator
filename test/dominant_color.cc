#include <iostream>

#include "../image_palette_generator.h"

int32_t main(int32_t argc, char** argv) {
  if (argc < 2) {
    std::cout << "Incorrect usage.\n"
              << "Example:\n"
              << argv[0] << " image.jpg" << std::endl;
    return -1;
  }
  ImagePaletteGenerator palette_generator;
  palette_generator.Open(argv[1]);
  std::cout << palette_generator.GetDominantColor().ToString() << "\n";
  return 0;
}
