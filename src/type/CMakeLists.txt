add_library(koishidb_type
        OBJECT
        slice.cc)
set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_type> PARENT_SCOPE)