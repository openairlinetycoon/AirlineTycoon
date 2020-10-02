cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR})

# Add source to this project's library.
add_library (SSE
    SSE.cpp
)

target_link_libraries(SSE
    SDL2::SDL2_ttf
    SDL2::SDL2_image
    SDL2::SDL2_mixer
    SDL2::SDL2)
