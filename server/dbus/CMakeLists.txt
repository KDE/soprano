project(soprano_server_dbus)

include_directories(
  ${soprano_server_dbus_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}
)

set(soprano_server_dbus_SRC
  dbusutil.cpp
  dbusmodeladaptor.cpp
  iteratorwrapper.cpp
  dbusstatementiteratoradaptor.cpp
  dbusnodeiteratoradaptor.cpp
  dbusqueryresultiteratoradaptor.cpp
  dbusserveradaptor.cpp
  dbusoperators.cpp
)

set(soprano_client_dbus_SRC
  dbusutil.cpp
  dbusmodelinterface.cpp
  dbusstatementiteratorinterface.cpp
  dbusnodeiteratorinterface.cpp
  dbusqueryresultiteratorinterface.cpp
  dbusserverinterface.cpp
  dbusclient.cpp
  dbusclientmodel.cpp
  dbusclientnodeiteratorbackend.cpp
  dbusclientstatementiteratorbackend.cpp
  dbusclientqueryresultiteratorbackend.cpp
  dbusoperators.cpp
)

qt4_automoc(${soprano_server_dbus_SRC})
qt4_automoc(${soprano_client_dbus_SRC})

add_library(sopranoserver_dbus STATIC ${soprano_server_dbus_SRC})
add_library(sopranoclient_dbus STATIC ${soprano_client_dbus_SRC})

#target_link_libraries(
#  sopranoserver_dbus
#  ${QT_QTCORE_LIBRARY}
 # ${QT_QTDBUS_LIBRARY}
#  QtDBus
#  soprano
#)

#target_link_libraries(
#  sopranosclient_dbus
#  ${QT_QTCORE_LIBRARY}
#  ${QT_QTDBUS_LIBRARY}
#  QtDBus
#  soprano
#)

install(
  FILES 
  dbusclient.h
  DESTINATION include/soprano
)
