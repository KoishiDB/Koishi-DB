cmake_minimum_required(VERSION 3.10)

project(KoishiDB)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate the compile_commands.json
set(CMAKE_CXX_STANDARD 17) # we need to use cxx 17 features
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(KOISHIDB_BUILD_TEST "Build KoishiDB test (currently not enabled)" ON) # the option variable for enable the test

if(${KOISHIDB_BUILD_TEST})
    add_definitions(-DDEBUG)
endif()

# set include path
set(KOISHIDB_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)


# include path at the root level
include_directories(${KOISHIDB_SRC_INCLUDE_DIR})


# add subdirectories
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third_party)

# enable google test
enable_testing()