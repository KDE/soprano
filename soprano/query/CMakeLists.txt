project(soprano_core)

if (WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif (WIN32)
 
if (APPLE)
 set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
 set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (APPLE)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${CMAKE_SOURCE_DIR}/soprano
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

SET(query_SRCS
    query.cpp
    query.h
)

qt4_automoc(${query_SRCS})

ADD_LIBRARY(soprano SHARED ${query_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})
