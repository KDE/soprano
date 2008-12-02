project(soprano_sesame2)

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" STRLESS "2.6.2")
   set(JNI_INCLUDE_DIRS ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" STRLESS "2.6.2")

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_sesame2_BINARY_DIR}
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

qt4_automoc(${sesame2_backend_SRC})

add_library(soprano_sesame2backend SHARED ${sesame2_backend_SRC})

target_link_libraries(soprano_sesame2backend soprano ${JAVA_JVM_LIBRARY})

install(TARGETS soprano_sesame2backend
  LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
  RUNTIME DESTINATION bin/soprano
  ARCHIVE DESTINATION lib/soprano
  )

install(FILES
  openrdf-sesame-2.1.4-onejar.jar
  slf4j-api-1.5.5.jar
  slf4j-simple-1.5.5.jar
  SopranoSesame2Wrapper.class
  DESTINATION share/soprano/sesame2
  )

configure_file(sesame2backend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/sesame2backend.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/sesame2backend.desktop
  DESTINATION share/soprano/plugins
  )
