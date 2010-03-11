project(soprano_client)

include_directories(
  ${soprano_client_BINARY_DIR}
  ${soprano_server_SOURCE_DIR}
  ${soprano_server_dbus_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
)

set(soprano_client_SRC
  ${soprano_server_SOURCE_DIR}/datastream.cpp
  ${soprano_server_SOURCE_DIR}/socket.cpp
#  tcpclient.cpp
  localsocketclient.cpp
  clientconnection.h
  clientconnection.cpp
  clientmodel.cpp
  clientstatementiteratorbackend.cpp
  clientnodeiteratorbackend.cpp
  clientqueryresultiteratorbackend.cpp
  sparql/sparqlmodel.cpp
  sparql/sparqlqueryresult.cpp
  sparql/sparqlprotocol.cpp
  sparql/sparqlxmlresultparser.cpp
)

if(BUILD_DBUS_SUPPORT)
  set(soprano_client_SRC
    ${soprano_client_SRC}
    ${soprano_server_SOURCE_DIR}/dbus/dbusutil.cpp
    ${soprano_server_SOURCE_DIR}/dbus/dbusoperators.cpp
    dbus/dbusabstractinterface.cpp
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
    dbus/dbusstatementiterator.cpp
    dbus/dbusnodeiterator.cpp
    dbus/dbusqueryresultiterator.cpp
    )
endif(BUILD_DBUS_SUPPORT)

qt4_automoc(${soprano_client_SRC})

add_library(sopranoclient SHARED ${soprano_client_SRC})

set_target_properties(sopranoclient PROPERTIES
  VERSION 1.2.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_CLIENT_LIB
  INSTALL_NAME_DIR ${LIB_DESTINATION}
)

target_link_libraries(
  sopranoclient
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  ${QT_QTXML_LIBRARY}
  soprano
)
if(BUILD_DBUS_SUPPORT)
  target_link_libraries(
    sopranoclient
    ${QT_QTDBUS_LIBRARY})
endif(BUILD_DBUS_SUPPORT)

INSTALL(TARGETS sopranoclient
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)

install(
  FILES 
  tcpclient.h
  localsocketclient.h
  DESTINATION include/soprano
)

add_subdirectory(sparql)
if(BUILD_DBUS_SUPPORT)
  add_subdirectory(dbus)
endif(BUILD_DBUS_SUPPORT)
