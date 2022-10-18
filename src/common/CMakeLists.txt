add_library(koishidb_common
        OBJECT
        option.cc
        )

SET(ALL_OBEJCT_FIELS ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:koishidb_common> PARENT_SCOPE)