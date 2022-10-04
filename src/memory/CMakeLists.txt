add_library(
        koishidb_memory
        OBJECT
        hash.cc
        skiplist.cc
        extendible_hash.cc
)

# TARGET_OBJECTS
set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_memory> PARENT_SCOPE)