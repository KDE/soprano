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

find_program(
  IODBC_CONFIG
  NAMES iodbc-config
  )

if(IODBC_CONFIG)
  execute_process(
    COMMAND ${IODBC_CONFIG} --version
    OUTPUT_VARIABLE IODBC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

  if(IODBC_VERSION)
    # extract include paths from iodbc-config
    execute_process(
      COMMAND ${IODBC_CONFIG} --cflags
      OUTPUT_VARIABLE iodbc_LIBS_ARGS
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )

    if(iodbc_LIBS_ARGS)
      string(REPLACE " " ";" iodbc_LIBS_ARGS ${iodbc_LIBS_ARGS})
      foreach(_ARG ${iodbc_LIBS_ARGS})
        if(${_ARG} MATCHES "^-I")
          string(REGEX REPLACE "^-I" "" _ARG ${_ARG})
          list(APPEND iodbc_INCLUDE_DIRS ${_ARG})
        endif()
      endforeach()
    endif()

    # extract lib paths from iodbc-config
    execute_process(
      COMMAND ${IODBC_CONFIG} --libs
      OUTPUT_VARIABLE iodbc_CFLAGS_ARGS
      )
    string(REPLACE " " ";" iodbc_CFLAGS_ARGS ${iodbc_CFLAGS_ARGS})
    foreach(_ARG ${iodbc_CFLAGS_ARGS})
      if(${_ARG} MATCHES "^-L")
        string(REGEX REPLACE "^-L" "" _ARG ${_ARG})
        list(APPEND iodbc_LIBRARY_DIRS ${_ARG})
      endif()
    endforeach()
  endif()
endif()

find_path(IODBC_INCLUDE_DIR sql.h
  HINTS
  ${iodbc_INCLUDE_DIRS}
  )

find_library(IODBC_LIBRARIES NAMES iodbc
  HINTS
  ${iodbc_LIBRARY_DIRS}
  )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(IODBC
                      REQUIRED_VARS IODBC_INCLUDE_DIR IODBC_LIBRARIES
                      VERSION_VAR IODBC_VERSION
                      )

mark_as_advanced(IODBC_INCLUDE_DIR
                 IODBC_LIBRARIES
                )

if(iodbc_LIBS_ARGS)
  set(IODBC_DEFINITIONS ${iodbc_CFLAGS})
endif()
