project(soprano_raptor_serializer)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${RAPTOR_INCLUDE_DIR}
)

set(CMAKE_REQUIRED_INCLUDES ${RAPTOR_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${RAPTOR_LIBRARIES})

set(raptor_serializer_SRC
  raptorserializer.cpp
)

add_library(soprano_raptorserializer MODULE ${raptor_serializer_SRC})

target_link_libraries(soprano_raptorserializer soprano ${RAPTOR_LIBRARIES})

set_target_properties(soprano_raptorserializer PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORSERIALIZER_LIB
)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  install(TARGETS soprano_raptorserializer DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  install(TARGETS soprano_raptorserializer
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/soprano
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
    )
endif()

configure_file(raptorserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
