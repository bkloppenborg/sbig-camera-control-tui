cmake_minimum_required(VERSION 3.8.2)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(base_types
  device.cpp
  camera.cpp
  filter_wheel.cpp
)

target_link_libraries(base_types 
  common
  niad
)
  
target_include_directories(base_types
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
    
