cmake_minimum_required(VERSION 3.8)

project(Blazek)

# 调试模式
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# 指定源文件
file(GLOB_RECURSE SRC_FILES src/main.cpp)

add_executable(Blazek ${SRC_FILES})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

