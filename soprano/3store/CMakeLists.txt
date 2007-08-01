project(soprano_tstore)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)

find_package(TStore)

if(TSTORE_FOUND)

  include_directories(
    ${QT_QTCORE_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${QT_INCLUDE_DIR}
    ${soprano_core_SOURCE_DIR}
    ${soprano_tstore_BINARY_DIR}
    ${TSTORE_INCLUDE_DIR}
    ${CMAKE_INSTALL_PREFIX}/include
    )
  
  set(tstore_backend_SRC
    threestoreutil.cpp
    threestoreutil.h
    threestoreresultiterator.cpp
    threestorequeryresult.cpp
    threestorequeryresult.h
    threestoremodel.cpp
    threestoremodel.h
    threestorebackend.cpp
    threestorebackend.h
    threestorecore.h
    threestorecore.cpp
    )

  qt4_automoc(${tstore_backend_SRC})

  add_library(soprano_tstorebackend SHARED ${tstore_backend_SRC})

  ADD_DEFINITIONS(${TSTORE_CFLAGS})

  TARGET_LINK_LIBRARIES(soprano_tstorebackend soprano ${TSTORE_LIBRARIES})

  set_target_properties(soprano_tstorebackend PROPERTIES
    DEFINE_SYMBOL MAKE_TSTOREBACKEND_LIB
    )

  INSTALL(TARGETS soprano_tstorebackend
    LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
    RUNTIME DESTINATION bin/soprano
    ARCHIVE DESTINATION lib/soprano
    )

endif(TSTORE_FOUND)
