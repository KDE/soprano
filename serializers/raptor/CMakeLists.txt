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

install(TARGETS soprano_raptorserializer ${PLUGIN_INSTALL_DIR})

configure_file(raptorserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
