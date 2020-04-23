FIND_PROGRAM(MEMORYCHECK_COMMAND valgrind )
SET(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
SET(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppress.txt" )

FILE(GLOB_RECURSE src_tests "*.c")
FOREACH(_test_file IN LISTS src_tests)
    GET_FILENAME_COMPONENT(_prg_filename ${_test_file} NAME_WE)
    GET_FILENAME_COMPONENT(_src_filename ${_test_file} NAME)

    ADD_EXECUTABLE(test_${_prg_filename} "${_src_filename}")
    TARGET_INCLUDE_DIRECTORIES(test_${_prg_filename} PUBLIC "${PROJECT_SOURCE_DIR}/inc")
    TARGET_LINK_LIBRARIES(test_${_prg_filename} PRIVATE ${PROJECT_NAME})
    TARGET_COMPILE_OPTIONS(test_${_prg_filename} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

    ADD_TEST(NAME test_${_prg_filename} COMMAND test_${_prg_filename})
ENDFOREACH()

