add_library(
        koishidb_memory
        OBJECT
        hash.cc
        memtable.cc
)

# TARGET_OBJECTS
set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_memory> PARENT_SCOPE)