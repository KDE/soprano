project(soprano_tools)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_BINARY_DIR}
  ${soprano_SOURCE_DIR}
)

configure_file(soprano-tools-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano-tools-config.h)

if(QT5_BUILD)
  message(STATUS "reenable sopranocmd when sopranoclient is ported")
else()
  add_executable(sopranocmd sopranocmd.cpp modelmonitor.cpp)

  target_link_libraries(
    sopranocmd
    soprano
    sopranoclient
    ${CLUCENE_LIBRARY}
  )

  if(QT5_BUILD)
    target_link_libraries(sopranocmd ${Qt5Core_LIBRARIES})
  else()
    target_link_libraries(sopranocmd ${QT_QTCORE_LIBRARY})
  endif()

  if(SOPRANO_BUILD_INDEX_LIB)
    target_link_libraries(
      sopranocmd
      sopranoindex
    )
  endif()
  install(TARGETS sopranocmd DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()

add_executable(onto2vocabularyclass onto2vocabularyclass.cpp)

target_link_libraries(
  onto2vocabularyclass
  soprano
)

if(QT5_BUILD)
  target_link_libraries(onto2vocabularyclass ${Qt5Core_LIBRARIES})
else()
  target_link_libraries(onto2vocabularyclass ${QT_QTCORE_LIBRARY})
endif()

install(TARGETS onto2vocabularyclass DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
