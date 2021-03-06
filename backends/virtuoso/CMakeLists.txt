project(virtuoso)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_BINARY_DIR}
  ${soprano_SOURCE_DIR}
  ${soprano_SOURCE_DIR}/server
  ${IODBC_INCLUDE_DIR}
)

set(SRC
  virtuosomodel.cpp
  virtuosobackend.cpp
  virtuosocontroller.cpp
  virtuosotools.cpp
  virtuosoqueryresultiteratorbackend.cpp
  virtuosoconfigurator.cpp
  odbcenvironment.cpp
  odbcconnection.cpp
  odbcconnectionpool.cpp
  odbcqueryresult.cpp
  ${soprano_core_SOURCE_DIR}/sopranodirs.cpp
  ${soprano_SOURCE_DIR}/server/lockfile.cpp
)


add_library(soprano_virtuosobackend MODULE ${SRC})

target_link_libraries(soprano_virtuosobackend soprano)
if(WIN32)
  target_link_libraries(soprano_virtuosobackend odbc32)
else()
  target_link_libraries(soprano_virtuosobackend ${IODBC_LIBRARIES})
endif()

install(TARGETS soprano_virtuosobackend ${PLUGIN_INSTALL_DIR})

configure_file(virtuosobackend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/virtuosobackend.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/virtuosobackend.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )

if(SOPRANO_BUILD_TESTS)
    add_executable(odbctest
        test.cpp
        virtuosomodel.cpp
        virtuosobackend.cpp
        virtuosocontroller.cpp
        virtuosotools.cpp
        virtuosoqueryresultiteratorbackend.cpp
        virtuosoconfigurator.cpp
        odbcenvironment.cpp
        odbcconnection.cpp
        odbcconnectionpool.cpp
        odbcqueryresult.cpp
        ${soprano_core_SOURCE_DIR}/sopranodirs.cpp
        ${soprano_SOURCE_DIR}/server/lockfile.cpp
    )

    target_link_libraries(odbctest
        soprano
        ${QT_QTCORE_LIBRARY}
        ${IODBC_LIBRARIES}
    )
endif()
