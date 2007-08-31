# - Try to find Redland
# Once done this will define
#
#  REDLAND_FOUND       - system has Redland
#  REDLAND_LIBRARIES   - Link these to use REDLAND
#  REDLAND_DEFINITIONS - Compiler switches required for using REDLAND

# (c) 2007 Sebastian Trueg <trueg@kde.org>
#
# Based on FindFontconfig Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR)

  # in cache already
  set(REDLAND_FOUND TRUE)

else (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR and RASQUAL_INCLUDE_DIR)

  if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(redland redland)
  endif (NOT WIN32)

  # now a hack for win32 (only?)
  # soprano only includes <redland.h> instead <redland/redland.h>
  if(WIN32)
    find_path(REDLAND_INCLUDE_DIR_TMP redland/redland.h
      PATHS
      ${_REDLANDIncDir}
      /usr/X11/include
    )
    if(REDLAND_INCLUDE_DIR_TMP)
      set(REDLAND_INCLUDE_DIR ${REDLAND_INCLUDE_DIR_TMP}/redland CACHE PATH "Path to a file.")
    endif(REDLAND_INCLUDE_DIR_TMP)

    find_path(RASQAL_INCLUDE_DIR_TMP redland/rasqal.h
      PATHS
      ${redland_INCLUDE_DIRS}
      /usr/X11/include
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
      /usr/X11/include
    )
  endif(WIN32)

  find_library(REDLAND_LIBRARIES NAMES rdf librdf
    PATHS
    ${redland_LIBRARY_DIRS}
  )

  if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR)
    # FIXME: do a version check without pkgconfig which also works on Win32
    if(NOT redland_VERSION OR redland_VERSION STRGREATER "1.0.5" )
      set(REDLAND_FOUND TRUE)
    else(NOT redland_VERSION OR redland_VERSION STRGREATER "1.0.5" )
      message(STATUS "Redland version less than 1.0.6: ${redland_VERSION}")
    endif(NOT redland_VERSION OR redland_VERSION STRGREATER "1.0.5" )
  endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR)

  if (REDLAND_FOUND)
    if (NOT Redland_FIND_QUIETLY)
      message(STATUS "Found Redland: ${REDLAND_LIBRARIES}")
    endif (NOT Redland_FIND_QUIETLY)
  else (REDLAND_FOUND)
    if (Redland_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Redland")
    endif (Redland_FIND_REQUIRED)
  endif (REDLAND_FOUND)

#  mark_as_advanced(REDLAND_LIBRARIES)

endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR)
