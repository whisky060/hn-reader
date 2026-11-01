# hn-reader

A terminal reader for Hacker News, written in C++20 with [FTXUI](https://github.com/ArthurSonzogni/FTXUI).


> NOTE: this is a personal project so it is not actively maintained also 
some features where not tested in all operating systems (MAC especially)

## Features

- Browse the current top stories from the Hacker News API
- Read articles in the terminal or in a browser
- Browse comment threads
- Render images inside the terminal (requires ImageMagick or GraphicsMagick)
- Multiple built-in themes and support for custom themes


## Requirements

- CMake 3.14 or newer
- A C++20 compiler
- Git and network access during the first configure (dependencies are downloaded by CMake)
- ImageMagick or GraphicsMagick (optional; needed to render article images)

The following dependencies are fetched automatically by `CMakeLists.txt`:

- FTXUI 7.0.0
- ftxui-image-view-async
- nlohmann/json 3.12.0
- curl 8.21.0
- lexbor 3.0.0

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

```sh
./build/hn-reader
```

## Image support

Article images are rendered with `ftxui-image-view-async`, which decodes images
with CImg.

By default this project builds CImg without its native libjpeg/libpng backends.
For JPEG/PNG and most other formats, CImg therefore delegates decoding to an
external ImageMagick or GraphicsMagick binary. If neither is available, the app
still runs, but article images will not render.

- ImageMagick (recommended):
  - Debian/Ubuntu: `sudo apt install imagemagick`
  - Fedora: `sudo dnf install ImageMagick`
  - Arch: `sudo pacman -S imagemagick`
  - macOS: `brew install imagemagick`
  - Windows: `winget install ImageMagick.ImageMagick` (ImageMagick 7; make sure
    `magick.exe` is on `PATH`)
- GraphicsMagick (`gm`) can be used instead.

Remote image URLs are downloaded with `curl`, then `wget` (PowerShell is also
tried on Windows). `curl` ships with Windows 10+ and is normally available on
Linux/macOS.

### Native libjpeg/libpng support

Instead of relying on ImageMagick at runtime, CImg can decode JPEG/PNG natively.
Define `cimg_use_jpeg` and/or `cimg_use_png` when compiling
`ftxui-image-view`, and link the corresponding libjpeg/libpng libraries (libpng
also requires zlib). This project does not enable those flags by default. If
only one is enabled, the other format still falls back to ImageMagick or
GraphicsMagick. GIF always requires one of those external tools.

## Keybindings

| Key | Action |
| --- | --- |
| `Up` / `Down` | Navigate the story list |
| `Enter` | Open the selected article |
| `c` | Open the comments for the selected story |
| `Ctrl+O` | Open the current article in a browser |
| `Ctrl+T` | Open the theme menu |
| `Up` / `Down`, `j` / `k` | Scroll the article |
| `PgUp` / `PgDn` | Scroll one page |
| `Home` / `End` | Jump to the start or end |
| `Esc` | Go back |

## Themes

Press `Ctrl+T` to switch between the built-in themes:

`hn light`, `hn dark`, `dracula`, `solarized dark`, `solarized light`,
`ashen`, `rose pine`, `rose pine dawn`.

Custom themes are JSON files placed in the configuration directory:

- Linux: `$XDG_CONFIG_HOME/hn-reader/themes` or `~/.config/hn-reader/themes`
- Windows: `%APPDATA%\hn-reader\themes`

See [`themes/color_scheme.json`](themes/color_scheme.json) for the expected format.

## License

Original code in this repository is licensed under the [MIT License](LICENSE).

Portions of the project are based on third-party code and remain under their
original licenses:

- MarkdownFTXUI — MIT
- FTXUI — MIT
- Mozilla Readability — Apache-2.0

See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md) for the full notices and
[LICENSES/Apache-2.0.txt](LICENSES/Apache-2.0.txt) for the Apache License 2.0
text.

This repository contains source code only;
