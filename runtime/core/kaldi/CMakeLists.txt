cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(kaldi)

add_library(kaldi
base/kaldi-math.cc
decoder/lattice-faster-decoder.cc
decoder/lattice-faster-online-decoder.cc
lat/determinize-lattice-pruned.cc
lat/lattice-functions.cc
)

target_link_libraries(kaldi PUBLIC utils)
