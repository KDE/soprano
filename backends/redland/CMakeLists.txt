project(soprano_redland)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${soprano_SOURCE_DIR}/cmake/modules)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_redland_BINARY_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${RASQAL_INCLUDE_DIR}
  ${RAPTOR_INCLUDE_DIR}
  )

ADD_DEFINITIONS(${REDLAND_CFLAGS})

# static libs do not get compiled with -fPIC which is needed on amd64
# when we link to a shared lib
# FIXME: do an amd64 check
if(NOT WIN32)
  ADD_DEFINITIONS(-fPIC)
endif()

if(BUILD_REDLAND_BACKEND OR BUILD_RAPTOR_SERIALIZER)
  set(redland_core_SRC
    redlandworld.cpp
    redlandstatementiterator.cpp
    redlandqueryresult.cpp
    redlandmodel.cpp
    redlandnodeiteratorbackend.cpp
    multimutex.cpp
    )

  add_library(soprano_redlandcore STATIC ${redland_core_SRC})

  TARGET_LINK_LIBRARIES(soprano_redlandcore soprano ${REDLAND_LIBRARIES})
endif()


if(BUILD_REDLAND_BACKEND)
  if (RASQAL_VERSION VERSION_LESS "0.9.15")
    message(STATUS "WARNING! - SPARQL ASK queries are broken in rasqal <= 0.9.14. It is recommended to update to at least version 0.9.15.")
  endif()

  set(redland_backend_SRC
    redlandbackend.cpp
    )


  add_library(soprano_redlandbackend MODULE ${redland_backend_SRC})

  TARGET_LINK_LIBRARIES(soprano_redlandbackend soprano_redlandcore)

  set_target_properties(soprano_redlandbackend PROPERTIES
    DEFINE_SYMBOL MAKE_REDLANDBACKEND_LIB
    )

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_redlandbackend DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  INSTALL(TARGETS soprano_redlandbackend
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/soprano
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
    )
endif()

  configure_file(redlandbackend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/redlandbackend.desktop)

  INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/redlandbackend.desktop
    DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
    )
endif()



