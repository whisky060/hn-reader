cmake_minimum_required(VERSION 3.14)
project(hn-reader VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

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

if(WIN32)
  set(CURL_USE_SCHANNEL ON)
else()
  set(CURL_USE_OPENSSL ON)
endif()



FetchContent_MakeAvailable(curl)
find_package(curl) # probably with CONFIG REQUIRED

FetchContent_Declare(lexbor
  GIT_REPOSITORY https://github.com/lexbor/lexbor
  GIT_TAG v3.0.0
)

set(LEXBOR_BUILD_SHARED OFF)
set(LEXBOR_BUILD_TESTS OFF)
set(LEXBOR_BUILD_EXAMPLES OFF)

FetchContent_MakeAvailable(lexbor)

 
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(hn-reader ${SOURCES})
target_include_directories(hn-reader PRIVATE src)
target_link_libraries(hn-reader PRIVATE ftxui::screen ftxui::dom ftxui::component nlohmann_json::nlohmann_json CURL::libcurl lexbor_static)

