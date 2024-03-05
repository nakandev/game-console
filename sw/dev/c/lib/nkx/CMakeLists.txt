cmake_minimum_required(VERSION 3.27)
project(nkxlibrary LANGUAGES CXX)

set(RV_PREFIX /home/nyalry/nakan/work/study/riscv/gcc/build32/bin/riscv32-unknown-elf-)
# set(CMAKE_CXX_COMPILER ${RV_PREFIX}g++ CACHE string "c++ compiler" FORCE)
set(CMAKE_CXX_COMPILER ${RV_PREFIX}g++)

set(CMAKE_CXX_FLAGS "-O2 -march=rv32im -mabi=ilp32 -fno-use-cxa-atexit -fno-exceptions -fno-rtti")

set(NKX_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NKX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../hw/spec/memorymap.h DESTINATION ${NKX_INCLUDE_DIR})
include_directories(${NKX_INCLUDE_DIR})

add_library(nkxhw
  STATIC
  ${NKX_SRC_DIR}/hw/nkx_hw.cpp
)

add_library(nkxmw
  STATIC
  ${NKX_SRC_DIR}/mw/nkx_mw.cpp
  ${NKX_SRC_DIR}/mw/nkx_mw_video.cpp
  ${NKX_SRC_DIR}/mw/nkx_mw_audio.cpp
  ${NKX_SRC_DIR}/mw/nkx_mw_input.cpp
)

add_library(nkxstd
  STATIC
  ${NKX_SRC_DIR}/std/malloc.cpp
)

add_library(nkxsw
  STATIC
  ${NKX_SRC_DIR}/sw/nkx_sw.cpp
  ${NKX_SRC_DIR}/sw/nkx_sw_video.cpp
  ${NKX_SRC_DIR}/sw/nkx_sw_audio.cpp
  ${NKX_SRC_DIR}/sw/nkx_sw_input.cpp
)
