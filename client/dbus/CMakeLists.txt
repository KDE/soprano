project(soprano_client_dbus)

install(
  FILES
  dbusclient.h
  dbusmodel.h
  dbusnodeiterator.h
  dbusqueryresultiterator.h
  dbusstatementiterator.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
  COMPONENT Devel
)
