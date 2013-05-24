project(soprano_rasqalqueryparser)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${RASQAL_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
)

set(rasqalqueryparser_SRC
  rasqalqueryparser.cpp
  rasqalqueryparser.h
)

add_library(soprano_rasqalqueryparser MODULE ${rasqalqueryparser_SRC})

target_link_libraries(soprano_rasqalqueryparser soprano ${RAPTOR_LIBRARIES} ${RASQAL_LIBRARIES} ${REDLAND_LIBRARIES})

add_definitions(${REDLAND_CFLAGS})

set_target_properties(soprano_rasqalqueryparser PROPERTIES
        DEFINE_SYMBOL MAKE_RASQALQUERYPARSER_LIB
)

install(TARGETS soprano_rasqalqueryparser ${PLUGIN_INSTALL_DIR})

configure_file(rasqalqueryparser.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/rasqalqueryparser.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/rasqalqueryparser.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
