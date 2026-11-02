cmake_minimum_required(VERSION 3.14)
project(hn-reader VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG d120f349b7e7b50584ea79a75a2523a5a66a087f # v7.0.0
)

FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
  ftxui-image-view
  GIT_REPOSITORY https://github.com/orrnithogalum/ftxui-image-view-async.git
  GIT_TAG 1501b3263789b9b0a4aaa61143d78e02d48ad550 # main (pinned 2026-11-02)
)
FetchContent_MakeAvailable(ftxui-image-view)

set_target_properties(tiv PROPERTIES INTERFACE_LINK_LIBRARIES "")

if(TARGET ftxui-imageview-example)
  set_target_properties(ftxui-imageview-example PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

# Enable CImg's built-in image decoders when the corresponding system
# libraries are available. CImg is vendored without any cimg_use_* macros
# set, so without this it can only decode by shelling out to external
# tools (curl/wget + ImageMagick/GraphicsMagick), which silently turns
# into a blank image whenever those are missing or lack a delegate
# (e.g. WebP served on a URL ending in .jpeg). Everything here is
# optional: on a host without these libraries no macros are defined and
# the previous external-tool fallback behavior is kept.
# Homebrew roots so macOS builds find brew-installed image libraries
# without extra cmake flags; harmless on other platforms. Must come
# before the find_package() calls below.
if(APPLE)
  list(APPEND CMAKE_PREFIX_PATH /opt/homebrew /usr/local)
endif()

find_package(JPEG QUIET)
find_package(PNG QUIET)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(WEBP QUIET IMPORTED_TARGET libwebp)
endif()

set(CIMG_DEFS "")
set(CIMG_LIBS "")
if(TARGET ftxui-image-view)
  if(JPEG_FOUND)
    list(APPEND CIMG_DEFS cimg_use_jpeg)
    list(APPEND CIMG_LIBS JPEG::JPEG)
  endif()
  if(PNG_FOUND)
    list(APPEND CIMG_DEFS cimg_use_png)
    list(APPEND CIMG_LIBS PNG::PNG)
  endif()
  if(WEBP_FOUND)
    list(APPEND CIMG_DEFS cimg_use_webp)
    list(APPEND CIMG_LIBS PkgConfig::WEBP)
  endif()
  target_compile_definitions(ftxui-image-view PRIVATE ${CIMG_DEFS})
  target_link_libraries(ftxui-image-view PRIVATE ${CIMG_LIBS})
endif()

FetchContent_Declare(nlohmann
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG 55f93686c01528224f448c19128836e7df245f72 # v3.12.0
)

FetchContent_MakeAvailable(nlohmann)


FetchContent_Declare(curl
URL https://curl.se/download/curl-8.21.0.tar.gz
DOWNLOAD_EXTRACT_TIMESTAMP true
OVERRIDE_FIND_PACKAGE # this one
)

if(WIN32)
  set(CURL_USE_SCHANNEL ON)
else()
  set(CURL_USE_OPENSSL ON)
endif()



FetchContent_MakeAvailable(curl)
find_package(curl) # probably with CONFIG REQUIRED

# Use libcurl (vendored above) for CImg's image network loading instead
# of spawning an external curl/wget subprocess per image. Must come
# after the vendored curl target exists.
if(TARGET CURL::libcurl)
  target_compile_definitions(ftxui-image-view PRIVATE cimg_use_curl)
  target_link_libraries(ftxui-image-view PRIVATE CURL::libcurl)
endif()

FetchContent_Declare(lexbor
  GIT_REPOSITORY https://github.com/lexbor/lexbor
  GIT_TAG 2ae88a1c6b5261830eff73ee12bb3cdf805f3cfe # v3.0.0
)

set(LEXBOR_BUILD_SHARED OFF)
set(LEXBOR_BUILD_TESTS OFF)
set(LEXBOR_BUILD_EXAMPLES OFF)

FetchContent_MakeAvailable(lexbor)


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(hn-reader ${SOURCES})
target_include_directories(hn-reader PRIVATE src)
target_link_libraries(hn-reader
PRIVATE ftxui::screen
        ftxui::dom
        ftxui::component
        ftxui-image-view
        nlohmann_json::nlohmann_json
        CURL::libcurl
        lexbor_static
        )

add_executable(harness tools/harness.cpp src/model/ArticleScraper/ArticleScraper.cpp src/utility/utility.cpp)
target_include_directories(harness PRIVATE src)
target_link_libraries(harness
PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    CURL::libcurl
    lexbor_static
    nlohmann_json::nlohmann_json
)

add_executable(harness-article tools/harness_article.cpp src/model/ArticleScraper/ArticleScraper.cpp src/utility/utility.cpp)
target_include_directories(harness-article PRIVATE src)
target_link_libraries(harness-article
PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    ftxui-image-view
    CURL::libcurl
    lexbor_static
    nlohmann_json::nlohmann_json
)
