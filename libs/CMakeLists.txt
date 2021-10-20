add_subdirectory (enet)
target_include_directories(enet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/enet/include)
add_subdirectory (libsmacker)
add_subdirectory (raknet)