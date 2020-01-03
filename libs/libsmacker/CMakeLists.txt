cmake_minimum_required(VERSION 2.6)

project(libsmacker)

add_library(libsmacker
    smacker.c
    smk_bitstream.c
    smk_hufftree.c
)
target_include_directories(libsmacker PUBLIC ${PROJECT_SOURCE_DIR})

add_executable (driver
    driver.c
)

target_link_libraries(driver libsmacker)

add_executable (smk2avi
    smk2avi.c
)

target_link_libraries(smk2avi libsmacker)
