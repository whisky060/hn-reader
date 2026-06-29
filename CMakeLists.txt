cmake_minimum_required(VERSION 3.14)
project(hn-reader VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
 
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.0  # Replace with a version, tag, or commit hash
)
 
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(nlohmann
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.12.0  # Replace with a version, tag, or commit hash
)

FetchContent_MakeAvailable(nlohmann)

FetchContent_Declare(curl
    URL https://curl.se/download/curl-8.21.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE # this one
)

FetchContent_MakeAvailable(curl)
find_package(curl) # probably with CONFIG REQUIRED

 
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(hn-reader ${SOURCES})
target_include_directories(hn-reader PRIVATE src)
target_link_libraries(hn-reader PRIVATE ftxui::screen ftxui::dom ftxui::component nlohmann_json::nlohmann_json CURL::libcurl)

