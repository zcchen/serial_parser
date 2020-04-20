cmake_minimum_required(VERSION 3.5)

######################## Global CMake Flags Here #####################
INCLUDE(cmake/settings.cmake)
SET(DEBUG FALSE)
SET(RELEASE FALSE)
SET(PROJECT_NAME main)
SET(PROJECT_VERSION_MAJOR 0)
SET(PROJECT_VERSION_MINOR 1)
SET(PROJECT_VERSION_PATCH 0)
######################################################################
project( ${PROJECT_NAME}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
        LANGUAGES C
    )

######################## Path Settings ###############################
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
######################################################################

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
######################################################################

######################## Running Dependanies Settings ################
######################################################################

ADD_SUBDIRECTORY("inc/")
ADD_SUBDIRECTORY("src/")

INCLUDE(CTest)
ADD_SUBDIRECTORY("tests/")
