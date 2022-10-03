add_subdirectory(memory)

# add_libray must have files
add_library(koishidb STATIC ${ALL_OBJECT_FILES})

# target_link_libraries
target_link_libraries(koishidb koishidb_memory)



target_include_directories(koishidb PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
