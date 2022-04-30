#include <cstdio>

#include "image_palette_generator.h"

int32_t main(int32_t argc, char** argv) {
  if (argc < 2) {
    printf("Incorrect usage.\nExample:\n%s image.jpg\n", argv[0]);
    return EXIT_FAILURE;
  }
  ImagePaletteGenerator palette_generator;
  palette_generator.Open(argv[1]);
  printf("Dominant Color: %s\n",
         palette_generator.GetDominantColor().ToString().c_str());
  printf("Palette:\n");
  for (auto color : palette_generator.GetPalette()) {
    printf("%s\n", color.ToString().c_str());
  }
  return EXIT_SUCCESS;
}
