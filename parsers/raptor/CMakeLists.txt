project(soprano_raptor_parser)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${soprano_SOURCE_DIR}/cmake/modules)

find_package(Redland REQUIRED) 

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_raptor_parser_BINARY_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

set(raptor_parser_SRC
  raptorparser.cpp
)

qt4_automoc(${raptor_parser_SRC})

add_library(soprano_raptorparser SHARED ${raptor_parser_SRC})

TARGET_LINK_LIBRARIES(soprano_raptorparser soprano soprano_redlandcore)

ADD_DEFINITIONS(${REDLAND_FLAGS})

set_target_properties(soprano_raptorparser PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORPARSER_LIB
)

INSTALL(TARGETS soprano_raptorparser
	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)
