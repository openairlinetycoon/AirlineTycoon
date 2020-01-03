cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${SDL2_TTF_INCLUDE_DIR})

# Add source to this project's library.
add_library (TeakLibW
    Album.cpp
    Bitmap.cpp
    Debug.cpp
    Math.cpp
    TeakFile.cpp
    TeakStr.cpp
    TextRes.cpp
)
