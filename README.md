# image_palette_generator

Generate a color palette from an image or find dominant color. 

## Samples

<img src='https://user-images.githubusercontent.com/28951144/166862968-8074ff95-85f2-4f8b-a53e-08c4887a52c3.png' width='600'></img>
<img src='https://user-images.githubusercontent.com/28951144/166862993-34641cf6-1ea7-42f7-acdf-044d0022ff11.png' width='600'></img>

Currently, you can extract:
- **Dominant Color:** Most prominent color in the image.
- **Color Palette:** A set of catchy colors taken from the image, sorted according to vibrancy.
- **Color Tones:** A set of normalized colors taken from various areas of the image, sorted according to luminance.

## Example

_Currently only C++ API is available._

Instantiate the class.

```cpp
auto palette_generator = ImagePaletteGenerator{};
```

Open an image source.

```cpp
// A file path.
palette_generator.Open("/home/user/image.jpeg");
// A FILE*
FILE* file = fopen(argv[1], "rb");
palette_generator.Open(file);
// An encoded image buffer.
palette_generator.Open(static_cast<uint8_t>(buffer), 1024);
```

Extract dominant color, palette or tones.

```cpp
auto color = palette_generator.GetDominantColor();

std::vector<Color> palette = palette_generator.GetPalette();
for (const auto &color: palette) {
  std::cout << color.ToString() << "\n";
}

std::vector<Color> tones = palette_generator.GetTones();
for (const auto &color: tones) {
  std::cout << color.ToString() << "\n";
}
```

## License 

MIT License

Copyright © 2022, Hitesh Kumar Saini <<saini123hitesh@gmail.com>>

## Acknowledgements

Majority of internal work is leveraged using following open-source libraries:

- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- [stb_image_resize](https://github.com/nothings/stb/blob/master/stb_image_resize.h)

MIT License

Copyright © 2017 Sean Barrett

- [ExoQuant](https://github.com/exoticorn/exoquant)


MIT License

Copyright © 2004, Dennis Ranke
