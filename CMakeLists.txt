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

# Include boost in case target platform doesn't support uint128
INCLUDE(CheckCXXSourceCompiles)
check_cxx_source_compiles("
int main() {
  __uint128_t a = 0;
  return 0;
}" HAVE_UINT128)

if (NOT HAVE_UINT128)
  FetchContent_Declare(
    boostorg
    URL https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz
    URL_HASH SHA256=273f1be93238a068aba4f9735a4a2b003019af067b9c183ed227780b8f36062c
  )

  FetchContent_GetProperties(boostorg)
  if (NOT boostorg_POPULATED)
      FetchContent_Populate(boostorg)
  endif ()

  include_directories(${boostorg_SOURCE_DIR})

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(boostorg)
endif()

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Werror -Wall -Wextra -fno-strict-aliasing -fPIC")

set(CC_OPTIMIZE "-O3")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CC_OPTIMIZE}")

add_subdirectory(src)
