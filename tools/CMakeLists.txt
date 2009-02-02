project(soprano_tools)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_BINARY_DIR}
  ${soprano_tools_BINARY_DIR}
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
)

configure_file(soprano-tools-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano-tools-config.h)

add_executable(sopranocmd sopranocmd.cpp)

add_executable(onto2vocabularyclass onto2vocabularyclass.cpp)

target_link_libraries(
  sopranocmd
  sopranoclient
  ${QT_QTCORE_LIBRARY}
  soprano
)

target_link_libraries(
  onto2vocabularyclass
  soprano
  ${QT_QTCORE_LIBRARY}
)

INSTALL(TARGETS sopranocmd onto2vocabularyclass
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)
