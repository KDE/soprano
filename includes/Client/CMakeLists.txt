install(FILES
  DBusClient
  DBusModel
  DBusNodeIterator
  DBusQueryResultIterator
  DBusStatementIterator
  LocalSocketClient
  SparqlModel
  TcpClient
  DESTINATION ${INCLUDE_INSTALL_DIR}/Soprano/Client
  COMPONENT Devel
)
