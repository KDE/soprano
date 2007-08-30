project(soprano_server)

include_directories(
  ${soprano_server_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}
)

set(soprano_server_SRC
  servercore.h
  servercore.cpp
  serverconnection.h
  serverconnection.cpp
  randomgenerator.cpp
  operators.cpp
)

set(soprano_client_SRC
  client.h
  client.cpp
  clientmodel.cpp
  clientstatementiteratorbackend.cpp
  clientnodeiteratorbackend.cpp
  clientqueryresultiteratorbackend.cpp
  serverbackend.h
  serverbackend.cpp
  operators.cpp
)
  
qt4_automoc(${soprano_server_SRC})
qt4_automoc(${soprano_client_SRC})

add_library(sopranoserver SHARED ${soprano_server_SRC})
add_library(soprano_serverclientbackend SHARED ${soprano_client_SRC})

add_executable(sopranod sopranod.cpp)

set_target_properties(sopranoserver PROPERTIES
        VERSION 1.0.0
        SOVERSION 1
        DEFINE_SYMBOL MAKE_SOPRANO_LIB
	INSTALL_NAME_DIR ${LIB_DESTINATION}
)

target_link_libraries(
  sopranoserver
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  soprano
)

target_link_libraries(
  soprano_serverclientbackend
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  soprano
)

target_link_libraries(
  sopranod
  sopranoserver
)

INSTALL(TARGETS sopranoserver sopranod
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)

INSTALL(TARGETS soprano_serverclientbackend
	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)

install(
  FILES 
  servercore.h
  serverbackend.h
  DESTINATION include/soprano
)
