project(sopranoindex)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}
  ${CLUCENE_INCLUDE_DIR}
  ${CLUCENE_LIBRARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set( SOPRANO_INDEX_SRCS
  cluceneindex.cpp
  clucenedocumentwrapper.cpp
  cluceneutils.cpp
  indexfiltermodel.cpp
  wstring.cpp
  indexqueryresult.cpp
)

configure_file(clucene-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/clucene-config.h)

# CLucene requires exception support and has no support for visibility=hidden
# so we must use the default (i.e. public) value for -fvisibility
IF(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fvisibility=default")
ENDIF(NOT WIN32)

# FIXME: make sure that CLUcene is compiled with UCS2 support

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
  VERSION 1.0.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_LIB
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
  indexqueryresult.h
  DESTINATION include/soprano
)
