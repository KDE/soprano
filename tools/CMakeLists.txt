project(soprano_tools)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_BINARY_DIR}
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
)

set(sopranocmd_SRC
  sopranocmd.cpp
)

add_executable(sopranocmd sopranocmd.cpp)

target_link_libraries(
  sopranocmd
  sopranoclient
  ${QT_QTCORE_LIBRARY}
  soprano
)

INSTALL(TARGETS sopranocmd
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)
