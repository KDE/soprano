include(CheckIncludeFiles)

check_include_files("knep/knep.h;/home/kdedev/test/include/knep/knep.h" HAVE_KNEP_KNEP_H)

if (HAVE_KNEP_KNEP_H)
   set(KNEP_HEADERS_FOUND TRUE)
endif (HAVE_KNEP_KNEP_H)

if (KNEP_HEADERS_FOUND)
   FIND_LIBRARY(KNEP_LIBS NAMES knepclient
      PATHS
      /usr/lib
      /usr/local/lib
   )
endif (KNEP_HEADERS_FOUND)

if (KNEP_HEADERS_FOUND)
   set(KNEP_FOUND TRUE)
   set(KNEP_LIBS ${KNEP_LIBS})
   message(STATUS "Found KNepomuk: ${KNEP_LIBS}")
endif (KNEP_HEADERS_FOUND)

#MARK_AS_ADVANCED(KNEP_LIBS)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
