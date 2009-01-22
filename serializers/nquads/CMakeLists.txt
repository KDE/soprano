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

qt4_automoc(${nquadserializer_SRC})

add_library(soprano_nquadserializer SHARED ${nquadserializer_SRC})

TARGET_LINK_LIBRARIES(soprano_nquadserializer soprano)

INSTALL(TARGETS soprano_nquadserializer
	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)

configure_file(nquadserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/nquadserializer.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/nquadserializer.desktop
  DESTINATION share/soprano/plugins
  )
