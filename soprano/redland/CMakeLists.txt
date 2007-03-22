project(soprano_redland)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)

find_package(Redland REQUIRED) 

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_redland_BINARY_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
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
	LIBRARY DESTINATION lib/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)
