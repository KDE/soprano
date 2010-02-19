cmake_policy(SET CMP0003 NEW)

project(sopranoindex)

include_directories(
  ${soprano_SOURCE_DIR}/soprano
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
  ${CLUCENE_INCLUDE_DIR}
  ${CLUCENE_LIBRARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set( SOPRANO_INDEX_SRCS
  cluceneindex.cpp
  clucenedocumentwrapper.cpp
  cluceneutils.cpp
  indexfiltermodel.cpp
  tstring.cpp
  indexqueryhit.cpp
  indexqueryhititeratorbackend.cpp
  queryhitwrapperresultiteratorbackend.cpp
)

configure_file(clucene-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/clucene-config.h)

# CLucene requires exception support and has no support for visibility=hidden
# so we must use the default (i.e. public) value for -fvisibility
IF(NOT WIN32)
    IF(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
        if(__SOPRANO_HAVE_GCC_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
        endif(__SOPRANO_HAVE_GCC_VISIBILITY)
        if(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-visibility-inlines-hidden")
        endif(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(NOT WIN32)

qt4_automoc(${SOPRANO_INDEX_SRCS})

add_library(sopranoindex SHARED ${SOPRANO_INDEX_SRCS})

target_link_libraries(
  sopranoindex
  ${QT_QTCORE_LIBRARY}
  soprano
  ${CLUCENE_LIBRARY}
)

set_target_properties(
  sopranoindex
  PROPERTIES
  VERSION 1.1.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_INDEX_LIB
  INSTALL_NAME_DIR ${LIB_DESTINATION}
)

INSTALL(TARGETS sopranoindex
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)

install(
  FILES 
  indexfiltermodel.h
  cluceneindex.h
  indexqueryhit.h
  DESTINATION include/soprano
)
