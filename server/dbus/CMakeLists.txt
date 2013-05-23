project(soprano_server_dbus)

install(
  FILES 
  dbusexportmodel.h
  dbusexportiterator.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
)

install(
  FILES
  org.soprano.Model.xml
  org.soprano.NodeIterator.xml
  org.soprano.QueryResultIterator.xml
  org.soprano.Server.xml
  org.soprano.StatementIterator.xml
  DESTINATION ${DATA_INSTALL_DIR}/dbus-1/interfaces
)
