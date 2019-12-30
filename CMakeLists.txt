cmake_minimum_required (VERSION 3.5)
if (NOT DEFINED CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER /usr/bin/clang++-6.0)
endif()

project(StarkwareCryptoLib VERSION 0.1.0 LANGUAGES CXX)
include(CTest)
# Basic setup for gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
enable_testing()
include_directories("${CMAKE_SOURCE_DIR}/src")

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Werror -Wall -Wextra -fno-strict-aliasing -fPIC")

set(CC_OPTIMIZE "-O3")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CC_OPTIMIZE}")

add_subdirectory(src)
