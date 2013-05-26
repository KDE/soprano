project(soprano_redland)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${RASQAL_INCLUDE_DIR}
  ${RAPTOR_INCLUDE_DIR}
  )

add_definitions(${REDLAND_CFLAGS})

# static libs do not get compiled with -fPIC which is needed on amd64
# when we link to a shared lib
# FIXME: do an amd64 check
if(NOT WIN32)
  add_definitions(-fPIC)
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

  target_link_libraries(soprano_redlandcore soprano ${REDLAND_LIBRARIES})
endif()

if(BUILD_REDLAND_BACKEND)
  if (RASQAL_VERSION VERSION_LESS "0.9.15")
    message(STATUS "WARNING! - SPARQL ASK queries are broken in rasqal <= 0.9.14. It is recommended to update to at least version 0.9.15.")
  endif()

  set(redland_backend_SRC
    redlandbackend.cpp
    )

  add_library(soprano_redlandbackend MODULE ${redland_backend_SRC})

  target_link_libraries(soprano_redlandbackend soprano_redlandcore)

  set_target_properties(soprano_redlandbackend PROPERTIES
    DEFINE_SYMBOL MAKE_REDLANDBACKEND_LIB
    )

  install(TARGETS soprano_redlandbackend ${PLUGIN_INSTALL_DIR})

  configure_file(redlandbackend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/redlandbackend.desktop)

  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/redlandbackend.desktop
    DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
    )
endif()
