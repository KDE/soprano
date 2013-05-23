project(soprano_client_dbus)

install(
  FILES 
  dbusclient.h
  dbusmodel.h
  dbusstatementiterator.h
  dbusnodeiterator.h
  dbusqueryresultiterator.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
)
