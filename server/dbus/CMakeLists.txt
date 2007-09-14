project(soprano_server_dbus)

include_directories(
  ${soprano_server_dbus_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}
)

install(
  FILES 
  dbusclient.h
  dbusmodel.h
  DESTINATION include/soprano
)
