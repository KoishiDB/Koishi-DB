add_library(
        koishidb_disk
        OBJECT
        builder.cc
        block_builder.cc
        block_iterator.cc
        table_builder.cc
        writable_file.cc
        filterblock_builder.cc
)

# TARGET_OBJECTS
set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_disk> PARENT_SCOPE)