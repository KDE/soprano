project(soprano_server)

include_directories(
  ${soprano_server_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
)

set(soprano_server_SRC
  servercore.h
  servercore.cpp
  serverconnection.h
  serverconnection.cpp
  randomgenerator.cpp
  operators.cpp
  dbus/dbusutil.cpp
  dbus/dbusmodeladaptor.cpp
  dbus/iteratorwrapper.cpp
  dbus/modelwrapper.cpp
  dbus/dbusstatementiteratoradaptor.cpp
  dbus/dbusnodeiteratoradaptor.cpp
  dbus/dbusqueryresultiteratoradaptor.cpp
  dbus/dbusserveradaptor.cpp
  dbus/dbusoperators.cpp
)

set(soprano_client_SRC
  tcpclient.h
  tcpclient.cpp
  clientconnection.h
  clientconnection.cpp
  clientmodel.cpp
  clientstatementiteratorbackend.cpp
  clientnodeiteratorbackend.cpp
  clientqueryresultiteratorbackend.cpp
  operators.cpp
  dbus/dbusutil.cpp
  dbus/dbusmodelinterface.cpp
  dbus/dbusstatementiteratorinterface.cpp
  dbus/dbusnodeiteratorinterface.cpp
  dbus/dbusqueryresultiteratorinterface.cpp
  dbus/dbusserverinterface.cpp
  dbus/dbusclient.cpp
  dbus/dbusmodel.cpp
  dbus/dbusclientnodeiteratorbackend.cpp
  dbus/dbusclientstatementiteratorbackend.cpp
  dbus/dbusclientqueryresultiteratorbackend.cpp
  dbus/dbusoperators.cpp
)

# no unix sockets on windows
if(NOT WIN32)
  set(HAVE_UNIX_SOCKETS "yes")
  set(soprano_server_SRC
    ${soprano_server_SRC}
    socketserver.cpp
    socketdevice.cpp
  )
  set(soprano_client_SRC
    ${soprano_client_SRC}
    unixsocketclient.cpp
    socketdevice.cpp
  )
endif(NOT WIN32)

configure_file(soprano-server-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano-server-config.h)

#set(soprano_serverbackend_SRC
#  serverbackend.h
#  serverbackend.cpp
#)
  
qt4_automoc(${soprano_server_SRC})
qt4_automoc(${soprano_client_SRC})

#qt4_automoc(${soprano_serverbackend_SRC})

add_library(sopranoserver SHARED ${soprano_server_SRC})
add_library(sopranoclient SHARED ${soprano_client_SRC})
#add_library(soprano_serverclientbackend SHARED ${soprano_serverbackend_SRC})

add_executable(sopranod sopranod.cpp lockfile.cpp)

set_target_properties(sopranoserver PROPERTIES
  VERSION 1.0.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_SERVER_LIB
  INSTALL_NAME_DIR ${LIB_DESTINATION}
)

set_target_properties(sopranoclient PROPERTIES
  VERSION 1.0.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_CLIENT_LIB
  INSTALL_NAME_DIR ${LIB_DESTINATION}
)

target_link_libraries(
  sopranoserver
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  soprano
)

target_link_libraries(
  sopranoclient
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  soprano
)

#target_link_libraries(
#  soprano_serverclientbackend
#  sopranoclient
#)

target_link_libraries(
  sopranod
  sopranoserver
  soprano
)

INSTALL(TARGETS sopranoserver sopranoclient sopranod
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)

#INSTALL(TARGETS soprano_serverclientbackend
#	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
#	RUNTIME DESTINATION bin/soprano
#	ARCHIVE DESTINATION lib/soprano
#)

install(
  FILES 
  servercore.h
  tcpclient.h
  DESTINATION include/soprano
)

if(HAVE_UNIX_SOCKETS)
  install(
    FILES 
    unixsocketclient.h
    DESTINATION include/soprano
    )
endif(HAVE_UNIX_SOCKETS)

add_subdirectory(dbus)
