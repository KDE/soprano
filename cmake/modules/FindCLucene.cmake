#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLUCENE_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLUCENE_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLUCENE_LIBRARY      = the library to link against CLucene - DEPRECATED Use CLUCENE_LIBRARIES
# CLUCENE_LIBRARIES    = the libraries to link against CLucene
# CLUCENE_VERSION      = The CLucene version string
# CLucene_FOUND        = set to 1 if clucene is found
#

include(CheckSymbolExists)
include(FindLibraryWithDebug)

if(NOT CLUCENE_MIN_VERSION)
  set(CLUCENE_MIN_VERSION "0.9.16")
endif()

if(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)
  include(${PROJECT_CMAKE}/CLuceneConfig.cmake)
endif()

set(TRIAL_LIBRARY_PATHS
  $ENV{CLUCENE_HOME}/lib${LIB_SUFFIX}
  ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
  /usr/local/lib${LIB_SUFFIX}
  /usr/lib${LIB_SUFFIX}
  /sw/lib${LIB_SUFFIX}
  /usr/pkg/lib${LIB_SUFFIX}
  )

set(TRIAL_INCLUDE_PATHS
  $ENV{CLUCENE_HOME}/include
  ${CMAKE_INSTALL_PREFIX}/include
  /usr/local/include
  /usr/include
  /sw/include
  /usr/pkg/include
  )

find_library_with_debug(CLUCENE_LIBRARY
  WIN32_DEBUG_POSTFIX d
  NAMES clucene clucene-core
  PATHS ${TRIAL_LIBRARY_PATHS})

# On Linux, Soprano needs both clucene-core and clucene-shared
# CLUCENE_LIBRARIES contains both entries if available
set(CLUCENE_LIBRARIES ${CLUCENE_LIBRARY})

find_library(CLUCENE_SHARED_LIBRARY
  NAMES clucene-shared
  HINTS ${TRIAL_LIBRARY_PATHS}
)

if(CLUCENE_SHARED_LIBRARY)
  set(CLUCENE_LIBRARIES ${CLUCENE_LIBRARIES} ${CLUCENE_SHARED_LIBRARY})
endif()

if(CLUCENE_LIBRARIES)
  message(STATUS "Found CLucene: ${CLUCENE_LIBRARIES}")
endif()

find_path(CLUCENE_INCLUDE_DIR
  NAMES CLucene.h
  PATHS ${TRIAL_INCLUDE_PATHS})

if(CLUCENE_INCLUDE_DIR)
  message(STATUS "Found CLucene include dir: ${CLUCENE_INCLUDE_DIR}")
endif()

if(WIN32)
  set(TRIAL_LIBRARY_PATHS ${CLUCENE_INCLUDE_DIR})
endif()

set(CLUCENE_GOOD_VERSION TRUE)

get_filename_component(TRIAL_LIBRARY_DIR ${CLUCENE_LIBRARY} PATH)
find_path(CLUCENE_LIBRARY_DIR
  NAMES CLucene/clucene-config.h PATHS ${TRIAL_LIBRARY_DIR} ${TRIAL_LIBRARY_PATHS} ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
if(CLUCENE_LIBRARY_DIR)
  message(STATUS "Found CLucene library dir: ${CLUCENE_LIBRARY_DIR}")
  file(READ ${CLUCENE_LIBRARY_DIR}/CLucene/clucene-config.h CLCONTENT)
  string(REGEX MATCH "_CL_VERSION +\".*\"" CLMATCH ${CLCONTENT})
  if(CLMATCH)
    string(REGEX REPLACE "_CL_VERSION +\"(.*)\"" "\\1" CLUCENE_VERSION ${CLMATCH})
    if(CLUCENE_VERSION VERSION_LESS "${CLUCENE_MIN_VERSION}")
      message(FATAL_ERROR " CLucene version ${CLUCENE_VERSION} is less than the required minimum ${CLUCENE_MIN_VERSION}")
      set(CLUCENE_GOOD_VERSION FALSE)
    endif()
    if(CLUCENE_VERSION VERSION_EQUAL "0.9.17")
      message(FATAL_ERROR "CLucene version 0.9.17 is not supported.")
      set(CLUCENE_GOOD_VERSION FALSE)
    endif()
  endif()
endif()

if(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARIES AND CLUCENE_LIBRARY_DIR AND CLUCENE_GOOD_VERSION)
  set(CLucene_FOUND TRUE)
endif()

if(CLucene_FOUND)
  if(NOT CLucene_FIND_QUIETLY)
    message(STATUS "Found CLucene: ${CLUCENE_LIBRARIES}")
  endif()
else()
  if(CLucene_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find CLucene.")
  endif()
endif()

mark_as_advanced(
  CLUCENE_INCLUDE_DIR
  CLUCENE_LIBRARY_DIR
  CLUCENE_SHARED_LIBRARY
  CLUCENE_LIBRARY
  CLUCENE_LIBRARIES
  )
