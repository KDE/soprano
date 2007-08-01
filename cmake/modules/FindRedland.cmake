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
    include(UsePkgConfig)

    pkgconfig(redland _REDLANDIncDir _REDLANDLinkDir _REDLANDLinkFlags _REDLANDCflags)

    set(REDLAND_DEFINITIONS ${_REDLANDCflags} CACHE INTERNAL "The compilation flags for redland")


    pkgconfig(rasqal _RASQALIncDir _RASQALLinkDir _RASQALLinkFlags _RASQALCflags)

    set(RASQAL_DEFINITIONS ${_RASQALCflags} CACHE INTERNAL "The compilation flags for rasqal")
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
      ${_REDLANDIncDir}
      /usr/X11/include
    )
    if(RASQAL_INCLUDE_DIR_TMP)
      set(RASQAL_INCLUDE_DIR ${RASQAL_INCLUDE_DIR_TMP}/redland CACHE PATH "Path to a file.")
    endif(RASQAL_INCLUDE_DIR_TMP)
  else(WIN32)
    find_path(REDLAND_INCLUDE_DIR redland.h
      PATHS
      ${_REDLANDIncDir}
      /usr/X11/include
    )
    find_path(RASQAL_INCLUDE_DIR rasqal.h
      PATHS
      ${_RASQALIncDir}
      /usr/X11/include
    )
  endif(WIN32)

  find_library(REDLAND_LIBRARIES NAMES rdf librdf
    PATHS
    ${_REDLANDLinkDir}
  )

  if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR AND RASQAL_INCLUDE_DIR)
     set(REDLAND_FOUND TRUE)
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
