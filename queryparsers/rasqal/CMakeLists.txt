project(soprano_rasqalqueryparser)

#IF(REDLAND_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${soprano_SOURCE_DIR}/cmake/modules)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${RASQAL_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  ${soprano_rasqalqueryparser_BINARY_DIR}
  ${soprano_SOURCE_DIR}
)

set(rasqalqueryparser_SRC
  rasqalqueryparser.cpp
  rasqalqueryparser.h
)

qt4_automoc(${rasqalqueryparser_SRC})

add_library(soprano_rasqalqueryparser SHARED ${rasqalqueryparser_SRC})

TARGET_LINK_LIBRARIES(soprano_rasqalqueryparser soprano ${REDLAND_LIBRARIES})

ADD_DEFINITIONS(${REDLAND_CFLAGS})

set_target_properties(soprano_rasqalqueryparser PROPERTIES
        DEFINE_SYMBOL MAKE_RASQALQUERYPARSER_LIB
)

INSTALL(TARGETS soprano_rasqalqueryparser
	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)

#ENDIF(REDLAND_FOUND)
