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
  )

set(redland_core_SRC
  redlandutil.cpp
  redlandutil.h
  redlandworld.cpp
  redlandworld.h
  redlandstatementiterator.cpp
  redlandstatementiterator.h
  redlandqueryresult.cpp
  redlandqueryresult.h
  redlandmodel.cpp
  redlandmodel.h
  redlandnodeiteratorbackend.cpp
  multimutex.cpp
  )

set(redland_backend_SRC
  redlandbackend.cpp
  redlandbackend.h
  )

qt4_automoc(${redland_backend_SRC})

add_library(soprano_redlandcore STATIC ${redland_core_SRC})

add_library(soprano_redlandbackend SHARED ${redland_backend_SRC})

ADD_DEFINITIONS(${REDLAND_CFLAGS})

# static libs do not get compiled with -fPIC which is needed on amd64
# when we link to a shared lib
# FIXME: do an amd64 check
if(NOT WIN32)
  ADD_DEFINITIONS(-fPIC)
endif(NOT WIN32)

TARGET_LINK_LIBRARIES(soprano_redlandcore soprano ${REDLAND_LIBRARIES})

TARGET_LINK_LIBRARIES(soprano_redlandbackend soprano_redlandcore)

set_target_properties(soprano_redlandbackend PROPERTIES
  DEFINE_SYMBOL MAKE_REDLANDBACKEND_LIB
  )

INSTALL(TARGETS soprano_redlandbackend
  LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
  RUNTIME DESTINATION bin/soprano
  ARCHIVE DESTINATION lib/soprano
  )

configure_file(redlandbackend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/redlandbackend.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/redlandbackend.desktop
  DESTINATION share/soprano/plugins
  )
