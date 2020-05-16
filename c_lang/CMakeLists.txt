cmake_minimum_required(VERSION 3.5)

######################## Path Settings ###############################
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
MESSAGE(">>> CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
######################################################################

######################## Global CMake Flags Here #####################
INCLUDE(cmake/settings.cmake)
SET(DEBUG FALSE)
SET(RELEASE FALSE)
SET(PROJECT_NAME SerialParser)
SET(PROJECT_VERSION_MAJOR 0)
SET(PROJECT_VERSION_MINOR 1)
SET(PROJECT_VERSION_PATCH 0)
######################################################################
project( ${PROJECT_NAME}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
        LANGUAGES C
    )

######################## Compiler Flags Settings #####################
IF (${DEBUG} AND ${RELEASE})
    MESSAGE(FATAL_ERROR "Flag DEBUG & RELEASE could not be both TRUE.")
ELSEIF (${DEBUG} AND NOT ${RELEASE})
    STRING(APPEND CMAKE_C_FLAGS " ${CMAKE_C_FLAGS_DEBUG}")
ELSEIF (NOT ${DEBUG} AND ${RELEASE})
    STRING(APPEND CMAKE_C_FLAGS " ${CMAKE_C_FLAGS_RELEASE}")
ENDIF ()
######################################################################

################ Options you will add here ###########################
######################################################################

######################## Building Dependanies Settings ###############
INCLUDE(cmake/cpack.cmake)
FIND_PACKAGE(Doxygen REQUIRED dot)
SET(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
SET(doxyfile ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
SET(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
CONFIGURE_FILE(${doxyfile_in} ${doxyfile} @ONLY)
ADD_CUSTOM_TARGET(docs
                  COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                  WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}
                  COMMENT "Generating API documentation with Doxygen"
                  DEPENDS build_docs_dirs
                  VERBATIM)
ADD_CUSTOM_TARGET(build_docs_dirs
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIRECTORY})
######################################################################

######################## Running Dependanies Settings ################
######################################################################

ADD_SUBDIRECTORY("inc/")
ADD_SUBDIRECTORY("src/")

INCLUDE(CTest)
ADD_SUBDIRECTORY("tests/")

