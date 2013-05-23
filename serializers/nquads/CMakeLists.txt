project(nquads_serializer)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${nquads_serializer_BINARY_DIR}
  ${QT_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
)

set(nquadserializer_SRC
  nquadserializer.cpp)


add_library(soprano_nquadserializer MODULE ${nquadserializer_SRC})

TARGET_LINK_LIBRARIES(soprano_nquadserializer soprano)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_nquadserializer DESTINATION bin/soprano)
else()
  INSTALL(TARGETS soprano_nquadserializer
    LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
    RUNTIME DESTINATION bin/soprano
    ARCHIVE DESTINATION lib/soprano
    )
endif()

configure_file(nquadserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/nquadserializer.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/nquadserializer.desktop
  DESTINATION share/soprano/plugins
  )
