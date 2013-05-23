project(soprano_raptor_serializer)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_raptor_serializer_BINARY_DIR}
  ${RAPTOR_INCLUDE_DIR}
)

set(CMAKE_REQUIRED_INCLUDES ${RAPTOR_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${RAPTOR_LIBRARIES})

set(raptor_serializer_SRC
  raptorserializer.cpp
)


add_library(soprano_raptorserializer MODULE ${raptor_serializer_SRC})

TARGET_LINK_LIBRARIES(soprano_raptorserializer soprano ${RAPTOR_LIBRARIES})

set_target_properties(soprano_raptorserializer PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORSERIALIZER_LIB
)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_raptorserializer DESTINATION bin/soprano)
else()
  INSTALL(TARGETS soprano_raptorserializer
    LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
    RUNTIME DESTINATION bin/soprano
    ARCHIVE DESTINATION lib/soprano
    )
endif()

configure_file(raptorserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop
  DESTINATION share/soprano/plugins
  )
