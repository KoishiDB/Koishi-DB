add_library(koishidb_network
        OBJECT
        helper.cc
        parser.cpp
        )

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_network> PARENT_SCOPE)