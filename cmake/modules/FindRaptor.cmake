# - Try to find the Raptor RDF parsing library (http://librdf.org/raptor/)
# Once done this will define
#
#  RAPTOR_FOUND       - system has Raptor
#  RAPTOR_LIBRARIES   - Link these to use Raptor
#  RAPTOR_INCLUDE_DIR - Include directory for using Raptor
#  RAPTOR_DEFINITIONS - Compiler switches required for using Raptor

# (c) 2007-2011 Sebastian Trueg <trueg@kde.org>
# (c) 2011 Artem Serebriyskiy <v.for.vandal@gmail.com>
#
# Based on FindFontconfig Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindLibraryWithDebug)
include(MacroEnsureVersion)

if (RAPTOR_INCLUDE_DIR AND RAPTOR_LIBRARIES)

  # in cache already
  set(RAPTOR_FOUND TRUE)

else (RAPTOR_INCLUDE_DIR AND RAPTOR_LIBRARIES)


  if (NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(PC_RAPTOR QUIET raptor2)
    set(RAPTOR_DEFINITIONS ${PC_RAPTOR_CFLAGS_OTHER})
    set(RAPTOR_VERSION ${PC_RAPTOR_VERSION})
  endif (NOT WIN32)


  find_library_with_debug(RAPTOR_LIBRARIES
                  WIN32_DEBUG_POSTFIX d
                  NAMES raptor2 
                  HINTS ${PC_RAPTOR_LIBDIR} ${PC_RAPTOR_LIBRARY_DIRS}
                  )

  find_path(RAPTOR_INCLUDE_DIR raptor.h
            HINTS ${PC_RAPTOR_INCLUDEDIR} ${PC_RAPTOR_INCLUDE_DIRS}
            PATH_SUFFIXES raptor2 )
  
  if (RAPTOR_VERSION)
    MACRO_ENSURE_VERSION("1.4.16" ${RAPTOR_VERSION} RAPTOR_HAVE_TRIG)
  endif (RAPTOR_VERSION)

  if (RAPTOR_FOUND)
    if (NOT Raptor_FIND_QUIETLY)
      message(STATUS "Found Raptor ${RAPTOR_VERSION}: libs - ${RAPTOR_LIBRARIES}; includes - ${RAPTOR_INCLUDE_DIR}")
    endif (NOT Raptor_FIND_QUIETLY)
  else (RAPTOR_FOUND)
    if (Raptor_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Raptor")
    endif (Raptor_FIND_REQUIRED)
  endif (RAPTOR_FOUND)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(RAPTOR  DEFAULT_MSG  RAPTOR_LIBRARIES RAPTOR_INCLUDE_DIR)

  mark_as_advanced(RAPTOR_INCLUDE_DIR RAPTOR_LIBRARIES)

endif (RAPTOR_INCLUDE_DIR AND RAPTOR_LIBRARIES)

