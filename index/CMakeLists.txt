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
    include(CheckCXXCompilerFlag)
    # Only GCC supports these flags
    IF(CMAKE_COMPILER_IS_GNUCXX)
        check_cxx_compiler_flag(-fvisibility=hidden __SOPRANO_HAVE_GCC_VISIBILITY)
        if(__SOPRANO_HAVE_GCC_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fvisibility=default")
        else(__SOPRANO_HAVE_GCC_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
        endif(__SOPRANO_HAVE_GCC_VISIBILITY)
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
  VERSION 2.0.0
  SOVERSION 2
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
