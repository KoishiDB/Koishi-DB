cmake_minimum_required(VERSION 3.10)

# include google test module
include(GoogleTest)

# set the files to generate test
file(GLOB_RECURSE KOISHIDB_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test.cc")

foreach(koishidb_test_source ${KOISHIDB_TEST_SOURCES})
    get_filename_component(koishidb_test_filename ${koishidb_test_source} NAME)

    string(REPLACE ".cc" "" koishidb_test_outputname ${koishidb_test_filename})

    add_executable(${koishidb_test_outputname} EXCLUDE_FROM_ALL ${koishidb_test_source})

    gtest_discover_tests(${koishidb_test_outputname})

    target_link_libraries(${koishidb_test_outputname} koishidb gtest gmock_main)


endforeach()
