# hn-reader

A Linux terminal reader for Hacker News, written in C++20 with [FTXUI](https://github.com/ArthurSonzogni/FTXUI).

> NOTE: this is a personal project, so it is not actively maintained.

## Features

- Browse the current top stories from the Hacker News API
- Read articles in the terminal or in a browser
- Browse comment threads
- Render images inside the terminal (using native JPEG/PNG/WebP support when
  available, with ImageMagick or GraphicsMagick as a fallback)
- Multiple built-in themes and support for custom themes

### Article List
![article list](docs/images/article_list_img.png)

### Article Example
![article example](docs/images/article_img.png)

### User Comments
![comments example](docs/images/comments_view_img.png)

### Image Example
![image example](docs/images/image_example_img.png)

## Limitations

- The [CMake file](CMakeLists.txt) works only for Linux
- Videos, GIFs, and PDFs are not supported
- Some websites posted on Hacker News may not be rendered correctly
- Code sections in articles are often not rendered correctly because they are
  not embedded in real code blocks, so they are treated as regular text
- Some lag may be experienced on image-heavy websites
- Unusual HTML combinations are not rendered correctly

## Requirements

- CMake 3.24 or newer
- A C++20 compiler
- Git and network access during the first configure (dependencies are downloaded
  by CMake)
- ImageMagick or GraphicsMagick (optional; fallback for image formats without
  native CImg support)
  - ImageMagick (fallback):
  - Debian/Ubuntu: `sudo apt install imagemagick`
  - Fedora: `sudo dnf install ImageMagick`
  - Arch: `sudo pacman -S imagemagick`

- Optional: libjpeg, libpng, and libwebp development packages for native image
  decoding

On Windows, you must use the same compiler toolchain that this project's CMake
build is configured with: the MSYS2 UCRT64 MinGW-w64
`g++` compiler with the `MinGW Makefiles` generator. Other compilers and
toolchains (MSVC, other MinGW variants, Clang) are not supported.

### Dependencies

The following dependencies are fetched automatically by `CMakeLists.txt`:

- FTXUI 7.0.0
- ftxui-image-view-async
- nlohmann/json 3.12.0
- curl 8.21.0
- lexbor 3.0.0

## Build

```sh
cmake --build build
```

## Run

```sh
./build/hn-reader
```

## Debugging the article scraper

The `harness` target is a small standalone tool used to debug
`ArticleScraper` without launching the full TUI. It fetches the page
with `curl`, runs it through the scraper pipeline with debug logging
enabled, and prints the scraper logs to stdout (progress and errors go
to stderr).

```sh
./build/harness "<url>"
```

The `harness-article` target renders the same scrape result in a full
terminal UI by reusing the app's real `ArticleView` (scrolling, Esc to
exit, Ctrl+O to open the url in a browser), so it is useful to debug
the rendered output rather than the scraper pipeline. Scraping runs
synchronously before the UI starts; a failed scrape is reported on
stderr and the tool exits.

```sh
./build/harness-article "<url>"
```

## Image support

Article images are rendered with `ftxui-image-view-async`, which decodes images
with CImg.

During configuration, CMake looks for the system libjpeg, libpng, and libwebp
libraries. When found, CImg is built with its native decoders enabled, so JPEG,
PNG, and WebP images are decoded without external tools. This happens
automatically; no extra CMake flags are needed.

If a decoder is not available, CImg falls back to an external ImageMagick or
GraphicsMagick binary. If neither is available, the app still runs, but images
in that format will not render.

- ImageMagick (fallback):
  - Debian/Ubuntu: `sudo apt install imagemagick`
  - Fedora: `sudo dnf install ImageMagick`
  - Arch: `sudo pacman -S imagemagick`

- GraphicsMagick (`gm`) can be used instead.

Remote image URLs are downloaded with the vendored libcurl (`cimg_use_curl`).

### Native JPEG/PNG/WebP support

The build enables CImg's `cimg_use_jpeg`, `cimg_use_png`, and `cimg_use_webp`
definitions automatically when the corresponding system libraries are found
(libpng also requires zlib). If only some of them are available, the missing
formats fall back to ImageMagick or GraphicsMagick. GIF always requires one of
those external tools.

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

Custom themes are JSON files placed in the configuration directory. The file
name without the `.json` extension becomes the theme name.

- Linux: `$XDG_CONFIG_HOME/hn-reader/themes` or `~/.config/hn-reader/themes`
- 
See [`themes/color_scheme.json`](themes/color_scheme.json) for the expected
format. It uses the keys `background`, `foreground`, `primary`, `secondary`,
`border`, `focus_background`, and `focus_foreground`, with colors written as
`#RRGGBB`.

## How Does It Work?

Content from the [Hacker News](https://news.ycombinator.com/) website is fetched
through the official [Hacker News API](https://github.com/HackerNews/API).

Articles are fetched using the link returned by the API. The main content is
extracted with a C++ port of the [Mozilla Readability
algorithm](https://github.com/mozilla/readability), using
[lexbor](https://lexbor.com/) as the HTML DOM helper.

The resulting HTML is parsed again and converted to a minimal subset of HTML,
which is then used by [FTXUI](https://github.com/ArthurSonzogni/FTXUI) to format
the article and make it more readable than plain text.

The same procedure, except for content extraction, is used to render user
comments.

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
