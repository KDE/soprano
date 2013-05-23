cmake_policy(SET CMP0003 NEW)

project(sopranoindex)

include_directories(
  ${soprano_SOURCE_DIR}/soprano
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
  ${CLUCENE_INCLUDE_DIR}
  ${CLUCENE_LIBRARY_DIR}
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
        endif()
        if(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-visibility-inlines-hidden")
        endif()
    ENDIF()
ENDIF()


add_library(sopranoindex SHARED ${SOPRANO_INDEX_SRCS})

target_link_libraries(
  sopranoindex
  ${QT_QTCORE_LIBRARY}
  soprano
  ${CLUCENE_LIBRARIES}
)

set_target_properties(
  sopranoindex
  PROPERTIES
  VERSION 1.1.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_INDEX_LIB
  INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
)

INSTALL(TARGETS sopranoindex EXPORT SopranoExport
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

install(
  FILES 
  indexfiltermodel.h
  cluceneindex.h
  indexqueryhit.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
)
