project(soprano_sesame2)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${JNI_INCLUDE_DIRS}
)

configure_file(sesame2-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/sesame2-config.h)

set(sesame2_backend_SRC
  jniwrapper.cpp
  jniobjectwrapper.cpp
  sesame2backend.cpp
  sesame2repository.cpp
  sesame2repositoryconnection.cpp
  sesame2valuefactory.cpp
  sesame2iterator.cpp
  sesame2model.cpp
  sesame2utils.cpp
  sesame2statementiteratorbackend.cpp
  sesame2nodeiteratorbackend.cpp
  sesame2queryresultiteratorbackend.cpp
  sesame2bindingset.cpp
  sesame2sopranowrapper.cpp
  jobjectref.cpp
  )

add_library(soprano_sesame2backend MODULE ${sesame2_backend_SRC})

target_link_libraries(soprano_sesame2backend soprano ${JAVA_JVM_LIBRARY})

install(TARGETS soprano_sesame2backend ${PLUGIN_INSTALL_DIR})

install(FILES
  openrdf-sesame-2.2.4-onejar.jar
  slf4j-api-1.5.5.jar
  slf4j-simple-1.5.5.jar
  SopranoSesame2Wrapper.class
  DESTINATION ${DATA_INSTALL_DIR}/soprano/sesame2
  )

configure_file(sesame2backend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/sesame2backend.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/sesame2backend.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
