project(soprano_raptor_parser)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_raptor_parser_BINARY_DIR}
  ${RAPTOR_INCLUDE_DIR}
)

set(raptor_parser_SRC
  raptorparser.cpp
)


add_library(soprano_raptorparser  MODULE ${raptor_parser_SRC})

TARGET_LINK_LIBRARIES(soprano_raptorparser soprano ${RAPTOR_LIBRARIES})

set_target_properties(soprano_raptorparser PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORPARSER_LIB
)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_raptorparser DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  INSTALL(TARGETS soprano_raptorparser
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/soprano
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
    )
endif()

configure_file(raptorparser.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorparser.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorparser.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
