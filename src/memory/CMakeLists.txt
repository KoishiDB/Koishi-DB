add_library(
        koishidb_memory
        OBJECT
        hash.cc
        extendible_hash.cc
        memtable.cc
        write_batch.cc
)

# TARGET_OBJECTS
set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_memory> PARENT_SCOPE)