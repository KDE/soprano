project(soprano_raptor_parser)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${RAPTOR_INCLUDE_DIR}
)

set(raptor_parser_SRC
  raptorparser.cpp
)

add_library(soprano_raptorparser  MODULE ${raptor_parser_SRC})

target_link_libraries(soprano_raptorparser soprano ${RAPTOR_LIBRARIES})

set_target_properties(soprano_raptorparser PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORPARSER_LIB
)

install(TARGETS soprano_raptorparser ${PLUGIN_INSTALL_DIR})

configure_file(raptorparser.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorparser.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorparser.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
