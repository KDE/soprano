project(soprano_tools)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_BINARY_DIR}
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
  /usr/include/iodbc
)

# enable unicode in iODBC
add_definitions(-DUNICODE)

set(SRC
  test.cpp
  iodbcconnection.cpp
  iodbcqueryiteratorbackend.cpp
  iodbcstatementhandler.cpp
  iodbctools.cpp
  odbcmodel.cpp
)

qt4_automoc(${SRC})

add_executable(odbctest ${SRC})

target_link_libraries(
  odbctest
  ${QT_QTCORE_LIBRARY}
  soprano
  iodbc
)
