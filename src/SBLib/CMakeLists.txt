cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${SDL2_TTF_INCLUDE_DIR})

# Add source to this project's library.
add_library (SBLib
    source/CBitmap.cpp
    source/CCursor.cpp
    source/CFont.cpp
    source/GfxLib.cpp
    source/Network.cpp
    source/Tool2.cpp
)
target_include_directories(SBLib PUBLIC include)
target_link_libraries(SBLib enet)
