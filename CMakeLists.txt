# Optional: lets CLion and VS Code (CMake Tools) open this folder directly.
# The Makefile is the primary build and needs nothing installed.
cmake_minimum_required(VERSION 3.20)
project(mincut LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRCS
    stoer_wagner/stoer_wagner.cpp
    karger/karger.cpp
    karger_stein/karger_stein.cpp
    common/brute_force.cpp
    common/generators.cpp)

add_executable(tests tests/tests.cpp ${SRCS})
add_executable(bench bench/bench.cpp ${SRCS})

foreach(t tests bench)
  target_include_directories(${t} PRIVATE include common)
  if(NOT MSVC)
    target_compile_options(${t} PRIVATE -Wall -Wextra)
  endif()
endforeach()

if(NOT MSVC AND NOT WIN32)
  target_compile_options(tests PRIVATE -g -fsanitize=address,undefined)
  target_link_options(tests PRIVATE -fsanitize=address,undefined)
endif()
target_compile_options(bench PRIVATE -O2 -DNDEBUG)

enable_testing()
add_test(NAME tests COMMAND tests)
