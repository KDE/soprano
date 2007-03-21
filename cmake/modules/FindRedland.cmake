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


if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)

  # in cache already
  set(REDLAND_FOUND TRUE)

else (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)

  if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    include(UsePkgConfig)

    pkgconfig(redland _REDLANDIncDir _REDLANDLinkDir _REDLANDLinkFlags _REDLANDCflags)

    set(REDLAND_DEFINITIONS ${_REDLANDCflags} CACHE INTERNAL "The compilation flags for redland")
  endif (NOT WIN32)

  find_path(REDLAND_INCLUDE_DIR redland.h
    PATHS
    ${_REDLANDIncDir}
    /usr/X11/include
  )

  find_library(REDLAND_LIBRARIES NAMES rdf librdf
    PATHS
    ${_REDLANDLinkDir}
  )

  if (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)
     set(REDLAND_FOUND TRUE)
  endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)

  if (REDLAND_FOUND)
    if (NOT REDLAND_FIND_QUIETLY)
      message(STATUS "Found REDLAND: ${REDLAND_LIBRARIES}")
    endif (NOT REDLAND_FIND_QUIETLY)
  else (REDLAND_FOUND)
    if (REDLAND_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find REDLAND")
    endif (REDLAND_FIND_REQUIRED)
  endif (REDLAND_FOUND)

#  mark_as_advanced(REDLAND_LIBRARIES)

endif (REDLAND_LIBRARIES AND REDLAND_INCLUDE_DIR)
