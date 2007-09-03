project(soprano_sesame2)

find_package(JNI)

if(JAVA_INCLUDE_PATH)
  file(READ ${JAVA_INCLUDE_PATH}/jni.h jni_header_data)
  string(REGEX MATCH "JNI_VERSION_1_6" have_jin_version_1_6 ${jni_header_data})
  if(have_jin_version_1_6)

    message(STATUS "Found Java JNI: ${JAVA_INCLUDE_PATH}, ${JAVA_JVM_LIBRARY}")

    include_directories(
      ${QT_QTCORE_INCLUDE_DIR}
      ${CMAKE_SOURCE_DIR}
      ${QT_INCLUDE_DIR}
      ${soprano_core_SOURCE_DIR}
      ${soprano_core_BINARY_DIR}
      ${soprano_sesame2_BINARY_DIR}
      ${CMAKE_INSTALL_PREFIX}/include
      ${JAVA_INCLUDE_PATH}
      ${JAVA_INCLUDE_PATH2}
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
      jobjectref.cpp
      )

    qt4_automoc(${sesame2_backend_SRC})

    add_library(soprano_sesame2backend SHARED ${sesame2_backend_SRC})

    TARGET_LINK_LIBRARIES(soprano_sesame2backend soprano ${JAVA_JVM_LIBRARY})

    INSTALL(TARGETS soprano_sesame2backend
      LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
      RUNTIME DESTINATION bin/soprano
      ARCHIVE DESTINATION lib/soprano
      )

    INSTALL(FILES 
      openrdf-sesame-2.0-beta5-onejar.jar
      slf4j-api-1.4.2.jar
      slf4j-simple-1.4.2.jar
      DESTINATION share/soprano/sesame2
      )

  else(have_jin_version_1_6)
    message( "Need JRE version 1.6 or higher for the Sesame2 backend.")
  endif(have_jin_version_1_6)

else(JAVA_INCLUDE_PATH)
    message(STATUS "Could not find Java JNI")
endif(JAVA_INCLUDE_PATH)
