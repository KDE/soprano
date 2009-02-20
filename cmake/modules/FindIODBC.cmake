# - Try to find iODBC
# Once done this will define
#
#  IODBC_FOUND       - system has Iodbc
#  IODBC_LIBRARIES   - Link these to use IODBC
#  IODBC_DEFINITIONS - Compiler switches required for using IODBC
#  IODBC_VERSION     - The iodbc version string

# (c) 2008 Sebastian Trueg <trueg@kde.org>
#
# Based on FindFontconfig Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

FIND_PROGRAM(
  IODBC_CONFIG
  NAMES iodbc-config
  )

if(IODBC_CONFIG)
  EXECUTE_PROCESS(
    COMMAND ${IODBC_CONFIG} --version
    OUTPUT_VARIABLE IODBC_VERSION
    )
  if(IODBC_VERSION)
    STRING(REPLACE "\n" "" IODBC_VERSION ${IODBC_VERSION})
    # extract include paths from iodbc-config
    EXECUTE_PROCESS(
      COMMAND ${IODBC_CONFIG} --cflags
      OUTPUT_VARIABLE iodbc_LIBS_ARGS)
    STRING( REPLACE " " ";" iodbc_LIBS_ARGS ${iodbc_LIBS_ARGS} )
    FOREACH( _ARG ${iodbc_LIBS_ARGS} )
      IF(${_ARG} MATCHES "^-I")
        STRING(REGEX REPLACE "^-I" "" _ARG ${_ARG})
        STRING( REPLACE "\n" "" _ARG ${_ARG} )
        LIST(APPEND iodbc_INCLUDE_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-I")
    ENDFOREACH(_ARG)
    
    # extract lib paths from iodbc-config
    EXECUTE_PROCESS(
      COMMAND ${IODBC_CONFIG} --libs
      OUTPUT_VARIABLE iodbc_CFLAGS_ARGS)
    STRING( REPLACE " " ";" iodbc_CFLAGS_ARGS ${iodbc_CFLAGS_ARGS} )
    FOREACH( _ARG ${iodbc_CFLAGS_ARGS} )
      IF(${_ARG} MATCHES "^-L")
        STRING(REGEX REPLACE "^-L" "" _ARG ${_ARG})
        LIST(APPEND iodbc_LIBRARY_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-L")
    ENDFOREACH(_ARG)
  endif(IODBC_VERSION)
endif(IODBC_CONFIG)

find_path(IODBC_INCLUDE_DIR sql.h
  PATHS
  ${iodbc_INCLUDE_DIRS}
  )

find_library(IODBC_LIBRARIES NAMES iodbc
  PATHS
  ${iodbc_LIBRARY_DIRS}
  )

if (IODBC_LIBRARIES AND IODBC_INCLUDE_DIR)
#  set(IODBC_INCLUDE_DIR "${IODBC_INCLUDE_DIR}/iodbc")
  set(IODBC_FOUND TRUE)
endif (IODBC_LIBRARIES AND IODBC_INCLUDE_DIR)

if (IODBC_FOUND)
  set(IODBC_DEFINITIONS ${iodbc_CFLAGS})
  if (NOT IODBC_FIND_QUIETLY)
    message(STATUS "Found iODBC ${IODBC_VERSION}: libs - ${IODBC_LIBRARIES}; includes - ${IODBC_INCLUDE_DIR}")
  endif (NOT IODBC_FIND_QUIETLY)
else (IODBC_FOUND)
  if (IODBC_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Iodbc")
  endif (IODBC_FIND_REQUIRED)
endif (IODBC_FOUND)
