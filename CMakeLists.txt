﻿cmake_minimum_required (VERSION 3.8)

project (AirlineTycoon)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT WIN32)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2-mixer REQUIRED)
find_package(SDL2-image REQUIRED)
find_package(SDL2-ttf REQUIRED)

# Include external libs.
add_subdirectory (libs)

# Include sub-projects.
add_subdirectory (src)
