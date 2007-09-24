project(soprano_raptor_serializer)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_raptor_serializer_BINARY_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

set(raptor_serializer_SRC
  raptorserializer.cpp
)

qt4_automoc(${raptor_serializer_SRC})

add_library(soprano_raptorserializer SHARED ${raptor_serializer_SRC})

TARGET_LINK_LIBRARIES(soprano_raptorserializer soprano soprano_redlandcore)

ADD_DEFINITIONS(${REDLAND_FLAGS})

set_target_properties(soprano_raptorserializer PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORSERIALIZER_LIB
)

INSTALL(TARGETS soprano_raptorserializer
	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)
