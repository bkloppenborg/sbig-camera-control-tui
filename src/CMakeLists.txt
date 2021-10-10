cmake_minimum_required(VERSION 3.8.2)

# build common tools
add_subdirectory(common)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Build base types and NIAD wrappers
add_subdirectory(base_types)

# Build manufacturer-specific interfaces
add_subdirectory(sbig)

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Core WebSockets)

# Build camera server
add_executable(camera-server
  main.cpp
  client.cpp
  worker.cpp
)
target_link_libraries(camera-server 
  Qt5::Core
  Qt5::WebSockets
  sbig
  niad
)
target_include_directories(camera-server
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
