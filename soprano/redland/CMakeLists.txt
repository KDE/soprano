project(soprano_redland)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  ${soprano_core_SOURCE_DIR}
  ${REDLAND_INCLUDE_DIR}
)

set(redland_backend_SRC
  redlandutil.cpp
  redlandworld.cpp
  redlandstatementiterator.cpp
  redlandqueryresult.cpp
  redlandmodel.cpp
  redlandparser.cpp
  redland_stream_adapter.c 
  redlandbackend.cpp
)    

qt4_automoc(${redland_backend_SRC})

add_library(soprano_redlandbackend SHARED ${redland_backend_SRC})

TARGET_LINK_LIBRARIES(soprano_redlandbackend soprano ${REDLAND_LIBRARIES})

ADD_DEFINITIONS(${REDLAND_CFLAGS})

set_target_properties(soprano_redlandbackend PROPERTIES
        DEFINE_SYMBOL MAKE_REDLANDBACKEND_LIB
)

INSTALL(TARGETS soprano_redlandbackend
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
)
