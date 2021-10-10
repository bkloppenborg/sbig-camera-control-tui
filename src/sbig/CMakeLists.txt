cmake_minimum_required(VERSION 3.8.2)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(SBIGUDRV REQUIRED)
find_package(CFITSIO REQUIRED)

add_library(sbig 
  sbig_st_driver.cpp
  sbig_st_device.cpp
  sbig_st_camera.cpp
  sbig_st_filter_wheel.cpp
  sbig_st_errors.cpp
  sbig_st_device_info.cpp
  sbig_st_readout_mode.cpp
)

target_link_libraries(sbig
  SBIGUDRV::SBIGUDRV 
  CFITSIO::CFITSIO 
  common
  base_types
)

target_include_directories(sbig
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
