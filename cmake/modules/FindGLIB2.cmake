# - Try to find the GLIB2 libraries
# Once done this will define
#
#  GLIB2_FOUND - system has glib2
#  GLIB2_INCLUDE_DIR - the glib2 include directory
#  GLIB2_LIBRARIES - glib2 library
#  GLIB2_CFLAGS

if(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES)
    # Already in cache, be silent
    set(GLIB2_FIND_QUIETLY TRUE)
endif()
IF (NOT WIN32)
   INCLUDE(UsePkgConfig)
   PKGCONFIG(glib-2.0 _LibGLIB2IncDir _LibGLIB2LinkDir _LibGLIB2LinkFlags GLIB2_CFLAGS)
endif()

FIND_PATH(GLIB2_INCLUDE_DIR glib.h
   ${_LibGLIB2IncDir}
   ${_LibGLIB2IncDir}/glib-2.0
   /usr/include/glib-2.0
   /usr/include
   /usr/local/include/glib-2.0
   /usr/local/include
)

FIND_LIBRARY(GLIB2_LIBRARIES NAMES glib-2.0
    PATHS
    ${_LibGLIB2LinkDir}
    /usr/lib
    /usr/local/lib
)

if(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES)
   set(GLIB2_FOUND TRUE)
endif()

if(GLIB2_FOUND)
   if(NOT GLIB2_FIND_QUIETLY)
      message(STATUS "Found glib2: ${GLIB2_LIBRARIES} ${GLIB2_LIBRARIES}")
   endif()
else()
   if (GLIB2_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find glib2 library")
   endif()
endif()

MARK_AS_ADVANCED(GLIB2_INCLUDE_DIR GLIB2_LIBRARIES)
