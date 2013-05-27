project(soprano_server)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_SOURCE_DIR}
)

set(soprano_server_SRC
  servercore.h
  servercore.cpp
  serverconnection.h
  serverconnection.cpp
  serverdatastream.cpp
  modelpool.cpp
  randomgenerator.cpp
  localserver.cpp
  tcpserver.cpp
)

if(BUILD_DBUS_SUPPORT)
  set(soprano_server_SRC
    ${soprano_server_SRC}
    dbus/dbusutil.cpp
    dbus/dbusmodeladaptor.cpp
    dbus/dbusexportiterator.cpp
    dbus/dbusexportmodel.cpp
    dbus/dbusstatementiteratoradaptor.cpp
    dbus/dbusnodeiteratoradaptor.cpp
    dbus/dbusqueryresultiteratoradaptor.cpp
    dbus/dbusserveradaptor.cpp
    dbus/dbuscontroller.cpp
    dbus/dbusoperators.cpp
    )
endif()

configure_file(soprano-server-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano-server-config.h)

add_library(sopranoserver ${LIBRARY_TYPE} ${soprano_server_SRC})

add_executable(sopranod sopranod.cpp sopranodcore.cpp lockfile.cpp)

if(WINCE)
    target_link_libraries(sopranod Coredll)
endif()

set_target_properties(sopranoserver PROPERTIES
  VERSION 1.2.0
  SOVERSION ${SOPRANO_GENERIC_SOVERSION}
  DEFINE_SYMBOL MAKE_SOPRANO_SERVER_LIB
  INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
)

target_link_libraries(
  sopranoserver
  soprano
)
if(QT5_BUILD)
  target_link_libraries(sopranoserver ${Qt5Core_LIBRARIES}
                                      ${Qt5Network_LIBRARIES}
                       )
else()
  target_link_libraries(sopranoserver ${QT_QTCORE_LIBRARY}
                                      ${QT_QTNETWORK_LIBRARY}
                       )
endif()

if(BUILD_DBUS_SUPPORT)
  if(QT5_BUILD)
    target_link_libraries(sopranoserver ${Qt5DBus_LIBRARIES})
  else()
    target_link_libraries(sopranoserver ${QT_QTDBUS_LIBRARY})
  endif()
endif()

target_link_libraries(
  sopranod
  soprano
  sopranoserver
  ${CLUCENE_LIBRARIES}
)

if(SOPRANO_BUILD_INDEX_LIB)
  target_link_libraries(
    sopranod
    sopranoindex
  )
endif()

install(TARGETS sopranoserver EXPORT SopranoExport ${INSTALL_TARGETS_DEFAULT_ARGS})

install(TARGETS sopranod ${INSTALL_TARGETS_DEFAULT_ARGS})

install(
  FILES
  servercore.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
  COMPONENT Devel
)

if(BUILD_DBUS_SUPPORT)
  add_subdirectory(dbus)
endif()
