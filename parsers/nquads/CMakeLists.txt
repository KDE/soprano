project(nquads_parser)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${nquads_parser_BINARY_DIR}
  ${QT_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
)

set(nquadparser_SRC
  nquadparser.cpp)


add_library(soprano_nquadparser MODULE ${nquadparser_SRC})

TARGET_LINK_LIBRARIES(soprano_nquadparser soprano)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_nquadparser DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  INSTALL(TARGETS soprano_nquadparser
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/soprano
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
    )
endif()

configure_file(nquadparser.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/nquadparser.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/nquadparser.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
