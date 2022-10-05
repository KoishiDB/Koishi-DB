add_library(
        koishidb_util
        OBJECT
        encode.cc
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} <TARGET:OBJECTS:koishdb_util> PARENT SCOPE)