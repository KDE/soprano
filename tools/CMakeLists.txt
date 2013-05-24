project(soprano_tools)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_BINARY_DIR}
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
)

configure_file(soprano-tools-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano-tools-config.h)

add_executable(sopranocmd sopranocmd.cpp modelmonitor.cpp)


add_executable(onto2vocabularyclass onto2vocabularyclass.cpp)

target_link_libraries(
  sopranocmd
  ${QT_QTCORE_LIBRARY}
  soprano
  sopranoclient
  ${CLUCENE_LIBRARY}
)
if(SOPRANO_BUILD_INDEX_LIB)
  target_link_libraries(
    sopranocmd
    sopranoindex
  )
endif()

target_link_libraries(
  onto2vocabularyclass
  ${QT_QTCORE_LIBRARY}
  soprano
)

INSTALL(TARGETS sopranocmd onto2vocabularyclass
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)
