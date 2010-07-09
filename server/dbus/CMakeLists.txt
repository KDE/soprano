project(soprano_server_dbus)

install(
  FILES 
  dbusexportmodel.h
  dbusexportiterator.h
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
