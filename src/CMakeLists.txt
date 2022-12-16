add_subdirectory(memory)
add_subdirectory(type)
add_subdirectory(util)
add_subdirectory(disk)
add_subdirectory(common)
add_subdirectory(network)

# add_libray must have files
add_library(koishidb STATIC ${ALL_OBJECT_FILES}
        db_impl.cc
        )

# target_link_libraries
target_link_libraries(koishidb
        koishidb_memory
        koishidb_type
        koishidb_util
        koishidb_disk
        koishidb_common
        koishidb_network
        )
target_include_directories(koishidb PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_executable(server main.cc)
target_link_libraries(server koishidb)