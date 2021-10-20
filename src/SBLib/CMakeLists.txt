cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR})

# Add source to this project's library.
add_library (SBLib
    source/CBitmap.cpp
    source/CCursor.cpp
    source/CFont.cpp
    source/GfxLib.cpp
    source/Network.cpp
    source/Tool2.cpp
    source/NetworkTypes/RAKNetNetwork.cpp
    )

target_include_directories(SBLib PUBLIC include)

target_link_libraries(SBLib
    raknet
    enet
    SDL2::SDL2_ttf
    SDL2::SDL2_image
    SDL2::SDL2_mixer
    SDL2::SDL2)
