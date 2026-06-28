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
 
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(hn-reader ${SOURCES})
target_include_directories(hn-reader PRIVATE src)
target_link_libraries(hn-reader PRIVATE ftxui::screen ftxui::dom ftxui::component)
