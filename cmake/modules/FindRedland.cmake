# - Try to find the Redland rdf libraries (http://librdf.org/)
# Once done this will define
#
#  REDLAND_FOUND       - system has Redland
#  REDLAND_LIBRARIES   - Link these to use REDLAND
#  REDLAND_INCLUDE_DIR - Include directory for using the redland library
#  REDLAND_DEFINITIONS - Compiler switches required for using REDLAND
#  REDLAND_VERSION     - The redland version string

# (c) 2007-2009 Sebastian Trueg <trueg@kde.org>
#
# Based on FindFontconfig Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


include(MacroEnsureVersion)

find_program(
  REDLAND_CONFIG_EXECUTABLE
  NAMES redland-config
  )

if(REDLAND_CONFIG_EXECUTABLE)
  EXECUTE_PROCESS(
    COMMAND ${REDLAND_CONFIG_EXECUTABLE} --version
    OUTPUT_VARIABLE REDLAND_VERSION
    )
  if(REDLAND_VERSION)
    string(REPLACE "\n" "" REDLAND_VERSION ${REDLAND_VERSION})
    
    # extract include paths from redland-config
    execute_process(
      COMMAND ${REDLAND_CONFIG_EXECUTABLE} --cflags
      OUTPUT_VARIABLE redland_LIBS_ARGS)
    string( REPLACE " " ";" redland_LIBS_ARGS ${redland_LIBS_ARGS} )
    foreach( _ARG ${redland_LIBS_ARGS} )
      if(${_ARG} MATCHES "^-I")
        string(REGEX REPLACE "^-I" "" _ARG ${_ARG})
        string( REPLACE "\n" "" _ARG ${_ARG} )
        list(APPEND redland_INCLUDE_DIRS ${_ARG})
      endif(${_ARG} MATCHES "^-I")
    endforeach(_ARG)
    
    # extract lib paths from redland-config
    execute_process(
      COMMAND ${REDLAND_CONFIG_EXECUTABLE} --libs
      OUTPUT_VARIABLE redland_CFLAGS_ARGS)
    string( REPLACE " " ";" redland_CFLAGS_ARGS ${redland_CFLAGS_ARGS} )
    foreach( _ARG ${redland_CFLAGS_ARGS} )
      if(${_ARG} MATCHES "^-L")
        string(REGEX REPLACE "^-L" "" _ARG ${_ARG})
        list(APPEND redland_LIBRARY_DIRS ${_ARG})
      endif(${_ARG} MATCHES "^-L")
    endforeach(_ARG)
  endif(REDLAND_VERSION)
endif(REDLAND_CONFIG_EXECUTABLE)


find_path(REDLAND_INCLUDE_DIR redland.h
  HINTS
  ${redland_INCLUDE_DIRS}
  /usr/X11/include
  PATH_SUFFIXES redland
  )

find_library(REDLAND_LIBRARIES NAMES rdf librdf
  HINTS
  ${redland_LIBRARY_DIRS}
  )

if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)
  set(REDLAND_FOUND TRUE)
endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)

if (REDLAND_FOUND)
  set(REDLAND_DEFINITIONS ${redland_CFLAGS})
  if (NOT Redland_FIND_QUIETLY)
    message(STATUS "Found Redland ${REDLAND_VERSION}: libs - ${REDLAND_LIBRARIES}; includes - ${REDLAND_INCLUDE_DIR}")
  endif (NOT Redland_FIND_QUIETLY)
else (REDLAND_FOUND)
  if (Redland_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Redland")
  endif (Redland_FIND_REQUIRED)
endif (REDLAND_FOUND)

mark_as_advanced(REDLAND_INCLUDE_DIR_TMP
  REDLAND_INCLUDE_DIR
  REDLAND_LIBRARIES)
