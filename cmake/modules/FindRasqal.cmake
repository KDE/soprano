# - Try to find the Rasqal rdf query library (http://librdf.org/rasqal/)
# Once done this will define
#
#  RASQAL_FOUND       - system has Rasqal
#  RASQAL_LIBRARIES   - Link these to use RASQAL
#  RASQAL_INCLUDE_DIR - The include directory for using rasqal
#  RASQAL_DEFINITIONS - Compiler switches required for using RASQAL
#  RASQAL_VERSION     - The rasqal version string

# (c) 2007-2009 Sebastian Trueg <trueg@kde.org>
#
# Based on FindFontconfig Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(WINCE)
  find_program(
    RASQAL_CONFIG
    NAMES rasqal-config
    PATHS ${HOST_BINDIR} NO_DEFAULT_PATH
    )
else()
  find_program(
    RASQAL_CONFIG
    NAMES rasqal-config
    )
endif()

  if(RASQAL_CONFIG)
    execute_process(
      COMMAND ${RASQAL_CONFIG} --version
      OUTPUT_VARIABLE RASQAL_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(RASQAL_VERSION)
      # extract include paths from rasqal-config
      execute_process(
        COMMAND ${RASQAL_CONFIG} --cflags
        OUTPUT_VARIABLE rasqal_CFLAGS_ARGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      string(REPLACE " " ";" rasqal_CFLAGS_ARGS ${rasqal_CFLAGS_ARGS})
      foreach(_ARG ${rasqal_CFLAGS_ARGS})
        if(${_ARG} MATCHES "^-I")
          string(REGEX REPLACE "^-I" "" _ARG ${_ARG})
          list(APPEND rasqal_INCLUDE_DIRS ${_ARG})
        endif()
      endforeach()

      # extract lib paths from rasqal-config
      execute_process(
        COMMAND ${RASQAL_CONFIG} --libs
        OUTPUT_VARIABLE rasqal_CFLAGS_ARGS)
      string(REPLACE " " ";" rasqal_CFLAGS_ARGS ${rasqal_CFLAGS_ARGS})
      foreach(_ARG ${rasqal_CFLAGS_ARGS})
        if(${_ARG} MATCHES "^-L")
          string(REGEX REPLACE "^-L" "" _ARG ${_ARG})
          list(APPEND rasqal_LIBRARY_DIRS ${_ARG})
        endif()
      endforeach()
    endif()
  endif()

  find_path(RASQAL_INCLUDE_DIR rasqal.h
    PATHS
    ${redland_INCLUDE_DIRS}
    ${rasqal_INCLUDE_DIRS}
    /usr/X11/include
    PATH_SUFFIXES redland rasqal
  )

  find_library(RASQAL_LIBRARIES NAMES rasqal librasqal
    PATHS
    ${rasqal_LIBRARY_DIRS}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
      Rasqal
      VERSION_VAR   RASQAL_VERSION
      REQUIRED_VARS RASQAL_LIBRARIES RASQAL_INCLUDE_DIR)

  if (RASQAL_FOUND)
    set(RASQAL_DEFINITIONS ${rasqal_CFLAGS})
  endif()

mark_as_advanced(RASQAL_INCLUDE_DIR_TMP
                 RASQAL_INCLUDE_DIR
                 RASQAL_LIBRARIES)
