cmake_minimum_required(VERSION 3.8.2)

find_package(Qt5 REQUIRED COMPONENTS Core)

# build common tools
add_library(common 
  common.cpp
  coordinate_conversions.cpp
  logging.cpp
)

target_link_libraries(common
  Qt5::Core
)

target_include_directories(common
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
