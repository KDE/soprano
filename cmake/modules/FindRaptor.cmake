# - Try to find the Raptor RDF parsing library (http://librdf.org/raptor/)
# Once done this will define
#
#  RAPTOR_FOUND       - system has Raptor
#  RAPTOR_LIBRARIES   - Link these to use Raptor
#  RAPTOR_INCLUDE_DIR - Include directory for using Raptor
#  RAPTOR_DEFINITIONS - Compiler switches required for using Raptor
#
#  Capabilities
#       RAPTOR_HAVE_TRIG   - Set if raptor has TRIG

# (c) 2007-2011 Sebastian Trueg <trueg@kde.org>
# (c) 2011 Artem Serebriyskiy <v.for.vandal@gmail.com>
# (c) 2011 Michael Jansen <kde@michael-jansen.biz>
#
# Based on FindFontconfig Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


MACRO ( FIND_RAPTOR libname libhints includehints )
    find_library_with_debug(
        RAPTOR_LIBRARIES
        WIN32_DEBUG_POSTFIX d
        NAMES ${libname}
        HINTS ${libhints})
    find_path(
        RAPTOR_INCLUDE_DIR raptor.h
        HINTS ${includehints}
        PATH_SUFFIXES ${libname})
ENDMACRO ()


# Check if we have cached results in case the last round was successful.
if ( NOT( RAPTOR_INCLUDE_DIR AND RAPTOR_LIBRARIES ) OR NOT RAPTOR_FOUND )

    include(FindLibraryWithDebug)
    include(MacroEnsureVersion)
    find_package(PkgConfig)

    if ( NOT WIN32 )
        pkg_check_modules(PC_RAPTOR2 QUIET raptor2)
        if ( PC_RAPTOR2_FOUND )
            set(RAPTOR_DEFINITIONS ${PC_RAPTOR2_CFLAGS_OTHER})
            set(RAPTOR_VERSION ${PC_RAPTOR2_VERSION} CACHE STRING "Raptor Version found" )
        endif ()
    endif ()
    find_raptor( raptor2 "${PC_RAPTOR2_LIBDIR};${PC_RAPTOR2_LIBRARY_DIRS}" "${PC_RAPTOR2_INCLUDEDIR};${PC_RAPTOR2_INCLUDE_DIRS}")

    mark_as_advanced(RAPTOR_INCLUDE_DIR RAPTOR_LIBRARIES)

endif () # Check for cached values

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    Raptor
    VERSION_VAR   RAPTOR_VERSION
    REQUIRED_VARS RAPTOR_LIBRARIES RAPTOR_INCLUDE_DIR)

mark_as_advanced(RAPTOR_VERSION)

if (NOT RAPTOR_FOUND )
    pkg_check_modules(PC_RAPTOR QUIET raptor)
    if (PC_RAPTOR_FOUND)
        message( STATUS "You have raptor1 version ${PC_RAPTOR_VERSION} installed. Please update." )
    endif ()
endif ()

