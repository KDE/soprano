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

qt4_automoc(${raptor_parser_SRC})

add_library(soprano_raptorparser  MODULE ${raptor_parser_SRC})

TARGET_LINK_LIBRARIES(soprano_raptorparser soprano ${RAPTOR_LIBRARIES})

set_target_properties(soprano_raptorparser PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORPARSER_LIB
)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_raptorparser DESTINATION bin/soprano)
else()
  INSTALL(TARGETS soprano_raptorparser
    LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
    RUNTIME DESTINATION bin/soprano
    ARCHIVE DESTINATION lib/soprano
    )
endif()

configure_file(raptorparser.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorparser.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorparser.desktop
  DESTINATION share/soprano/plugins
  )
