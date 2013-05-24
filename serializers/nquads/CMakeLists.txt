project(nquads_serializer)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
)

set(nquadserializer_SRC
  nquadserializer.cpp)

add_library(soprano_nquadserializer MODULE ${nquadserializer_SRC})

target_link_libraries(soprano_nquadserializer soprano)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  install(TARGETS soprano_nquadserializer DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  install(TARGETS soprano_nquadserializer
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/soprano
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
    )
endif()

configure_file(nquadserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/nquadserializer.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/nquadserializer.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
