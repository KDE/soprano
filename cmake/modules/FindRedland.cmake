# - Try to find Redland
# Once done this will define
#
#  REDLAND_FOUND       - system has Redland
#  REDLAND_LIBRARIES   - Link these to use REDLAND
#  REDLAND_DEFINITIONS - Compiler switches required for using REDLAND
#  REDLAND_VERSION     - The redland version string

# (c) 2007 Sebastian Trueg <trueg@kde.org>
#
# Based on FindFontconfig Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


#if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR AND RASQAL_LIBRARIES AND RAPTOR_LIBRARIES)

  # in cache already
 # set(REDLAND_FOUND TRUE)

#else (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR and RASQUAL_INCLUDE_DIR AND RASQAL_LIBRARIES AND RAPTOR_LIBRARIES)

  FIND_PROGRAM(
    REDLAND_CONFIG
    NAMES redland-config
    )

  if(REDLAND_CONFIG)
    EXECUTE_PROCESS(
      COMMAND redland-config --version
      OUTPUT_VARIABLE redland_VERSION
      )

    # extract include paths from redland-config
    EXECUTE_PROCESS(
      COMMAND redland-config --cflags
      OUTPUT_VARIABLE redland_LIBS_ARGS)
    STRING( REPLACE " " ";" redland_LIBS_ARGS ${redland_LIBS_ARGS} )
    FOREACH( _ARG ${redland_LIBS_ARGS} )
      IF(${_ARG} MATCHES "^-I")
        STRING(REGEX REPLACE "^-I" "" _ARG ${_ARG})
        LIST(APPEND redland_INCLUDE_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-I")
    ENDFOREACH(_ARG)

    # extract lib paths from redland-config
    EXECUTE_PROCESS(
      COMMAND redland-config --libs
      OUTPUT_VARIABLE redland_CFLAGS_ARGS)
    STRING( REPLACE " " ";" redland_CFLAGS_ARGS ${redland_CFLAGS_ARGS} )
    FOREACH( _ARG ${redland_CFLAGS_ARGS} )
      IF(${_ARG} MATCHES "^-L")
        STRING(REGEX REPLACE "^-L" "" _ARG ${_ARG})
        LIST(APPEND redland_LIBRARY_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-L")
    ENDFOREACH(_ARG)
  endif(REDLAND_CONFIG)

  # raptor is not redland, look for it separately
  FIND_PROGRAM(
    RAPTOR_CONFIG
    NAMES raptor-config
    )
  if(RAPTOR_CONFIG)
    # extract include paths from raptor-config
    EXECUTE_PROCESS(
      COMMAND raptor-config --cflags
      OUTPUT_VARIABLE raptor_CFLAGS_ARGS)
    STRING( REPLACE " " ";" raptor_CFLAGS_ARGS ${raptor_CFLAGS_ARGS} )
    FOREACH( _ARG ${raptor_CFLAGS_ARGS} )
      IF(${_ARG} MATCHES "^-I")
        STRING(REGEX REPLACE "^-I" "" _ARG ${_ARG})
        LIST(APPEND raptor_INCLUDE_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-I")
    ENDFOREACH(_ARG)

    # extract lib paths from raptor-config
    EXECUTE_PROCESS(
      COMMAND raptor-config --libs
      OUTPUT_VARIABLE raptor_CFLAGS_ARGS)
    STRING( REPLACE " " ";" raptor_CFLAGS_ARGS ${raptor_CFLAGS_ARGS} )
    FOREACH( _ARG ${raptor_CFLAGS_ARGS} )
      IF(${_ARG} MATCHES "^-L")
        STRING(REGEX REPLACE "^-L" "" _ARG ${_ARG})
        LIST(APPEND raptor_LIBRARY_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-L")
    ENDFOREACH(_ARG)

  endif(RAPTOR_CONFIG)


  # rasqal is not redland, look for it separately
  FIND_PROGRAM(
    RASQAL_CONFIG
    NAMES rasqal-config
    )

  if(RASQAL_CONFIG)
    # extract include paths from rasqal-config
    EXECUTE_PROCESS(
      COMMAND rasqal-config --cflags
      OUTPUT_VARIABLE rasqal_CFLAGS_ARGS)
    STRING( REPLACE " " ";" rasqal_CFLAGS_ARGS ${rasqal_CFLAGS_ARGS} )
    FOREACH( _ARG ${rasqal_CFLAGS_ARGS} )
      IF(${_ARG} MATCHES "^-I")
        STRING(REGEX REPLACE "^-I" "" _ARG ${_ARG})
        LIST(APPEND rasqal_INCLUDE_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-I")
    ENDFOREACH(_ARG)

    # extract lib paths from rasqal-config
    EXECUTE_PROCESS(
      COMMAND rasqal-config --libs
      OUTPUT_VARIABLE rasqal_CFLAGS_ARGS)
    STRING( REPLACE " " ";" rasqal_CFLAGS_ARGS ${rasqal_CFLAGS_ARGS} )
    FOREACH( _ARG ${rasqal_CFLAGS_ARGS} )
      IF(${_ARG} MATCHES "^-L")
        STRING(REGEX REPLACE "^-L" "" _ARG ${_ARG})
        LIST(APPEND rasqal_LIBRARY_DIRS ${_ARG})
      ENDIF(${_ARG} MATCHES "^-L")
    ENDFOREACH(_ARG)
  endif(RASQAL_CONFIG)

  # now a hack for win32 (only?)
  # soprano only includes <redland.h> instead <redland/redland.h>
  if(WIN32)
    find_path(REDLAND_INCLUDE_DIR_TMP redland/redland.h
      PATHS
      ${_REDLANDIncDir}
    )
    if(REDLAND_INCLUDE_DIR_TMP)
      set(REDLAND_INCLUDE_DIR ${REDLAND_INCLUDE_DIR_TMP}/redland CACHE PATH "Path to a file.")
    endif(REDLAND_INCLUDE_DIR_TMP)

    find_path(RASQAL_INCLUDE_DIR_TMP redland/rasqal.h
      PATHS
      ${redland_INCLUDE_DIRS}
    )
    if(RASQAL_INCLUDE_DIR_TMP)
      set(RASQAL_INCLUDE_DIR ${RASQAL_INCLUDE_DIR_TMP}/redland CACHE PATH "Path to a file.")
    endif(RASQAL_INCLUDE_DIR_TMP)
  else(WIN32)
    find_path(REDLAND_INCLUDE_DIR redland.h
      PATHS
      ${redland_INCLUDE_DIRS}
      /usr/X11/include
    )
    find_path(RASQAL_INCLUDE_DIR rasqal.h
      PATHS
      ${redland_INCLUDE_DIRS}
      ${rasqal_INCLUDE_DIRS}
      /usr/X11/include
    )
  endif(WIN32)

  find_library(REDLAND_LIBRARIES NAMES rdf librdf
    PATHS
    ${redland_LIBRARY_DIRS}
  )
 
  find_library(RASQAL_LIBRARIES NAMES rasqal librasqal
    PATHS
    ${rasqal_LIBRARY_DIRS}
  )
 
  find_library(RAPTOR_LIBRARIES NAMES raptor libraptor
    PATHS
    ${raptor_LIBRARY_DIRS}
  )

  MESSAGE(STATUS "redland libs: ${REDLAND_LIBRARIES}")
  MESSAGE(STATUS "redland includes: ${REDLAND_INCLUDE_DIR}")
  MESSAGE(STATUS "rasqal includes: ${RASQAL_INCLUDE_DIR}")
  MESSAGE(STATUS "rasqal libs: ${RASQAL_LIBRARIES}")
  MESSAGE(STATUS "raptor libs: ${RAPTOR_LIBRARIES}")

  if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR AND RASQAL_LIBRARIES AND RAPTOR_LIBRARIES)
    set(REDLAND_FOUND TRUE)
  endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR AND RASQAL_LIBRARIES AND RAPTOR_LIBRARIES)

  if (REDLAND_FOUND)
    set(REDLAND_DEFINITIONS ${redland_CFLAGS})
    set(REDLAND_VERSION ${redland_VERSION})
    if (NOT Redland_FIND_QUIETLY)
      message(STATUS "Found Redland: ${REDLAND_LIBRARIES}")
    endif (NOT Redland_FIND_QUIETLY)
  else (REDLAND_FOUND)
    if (Redland_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Redland")
    endif (Redland_FIND_REQUIRED)
  endif (REDLAND_FOUND)

#  mark_as_advanced(REDLAND_LIBRARIES)

#endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR AND RASQAL_LIBRARIES AND RAPTOR_LIBRARIES)
