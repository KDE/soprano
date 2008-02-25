project(soprano_server_dbus)

include_directories(
  ${soprano_server_dbus_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}
)

install(
  FILES 
  dbusclient.h
  dbusmodel.h
  dbusexportmodel.h
  DESTINATION include/soprano
)

install(
  FILES
  org.soprano.Model.xml
  org.soprano.NodeIterator.xml
  org.soprano.QueryResultIterator.xml
  org.soprano.Server.xml
  org.soprano.StatementIterator.xml
  DESTINATION share/dbus-1/interfaces
)
