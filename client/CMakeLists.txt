project(soprano_client)

include_directories(
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
  tcpclient.cpp
  socket.cpp
  socketstream.cpp
  localsocketclient.cpp
  clientconnection.h
  clientconnection.cpp
  clientmodel.cpp
  clientstatementiteratorbackend.cpp
  clientnodeiteratorbackend.cpp
  clientqueryresultiteratorbackend.cpp
)

if(NOT WINCE)
  set(soprano_client_SRC
    ${soprano_client_SRC}
    sparql/sparqlmodel.cpp
    sparql/sparqlqueryresult.cpp
    sparql/sparqlprotocol.cpp
    sparql/sparqlxmlresultparser.cpp
    )
endif()

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
endif()


add_library(sopranoclient ${LIBRARY_TYPE} ${soprano_client_SRC})

set_target_properties(sopranoclient PROPERTIES
  VERSION 1.2.0
  SOVERSION 1
  DEFINE_SYMBOL MAKE_SOPRANO_CLIENT_LIB
  INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
)

target_link_libraries(
  sopranoclient
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  ${QT_QTXML_LIBRARY}
  soprano
)

if(WIN32)
   target_link_libraries(sopranoclient ws2_32)
endif()

if(BUILD_DBUS_SUPPORT)
  target_link_libraries(
    sopranoclient
    ${QT_QTDBUS_LIBRARY})
endif()

install(TARGETS sopranoclient EXPORT SopranoExport
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

install(
  FILES
  localsocketclient.h
  tcpclient.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
  COMPONENT Devel
)

add_subdirectory(sparql)

if(BUILD_DBUS_SUPPORT)
  add_subdirectory(dbus)
endif()
