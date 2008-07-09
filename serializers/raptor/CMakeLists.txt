project(soprano_raptor_serializer)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_raptor_serializer_BINARY_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${RAPTOR_INCLUDE_DIR}
)

include(CheckFunctionExists)
set(CMAKE_REQUIRED_INCLUDES ${RAPTOR_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${RAPTOR_LIBRARIES})
check_function_exists( raptor_new_iostream_from_handler2 HAVE_IOSTREAM_HANDLER2 )
if(HAVE_IOSTREAM_HANDLER2)
  add_definitions(-DHAVE_IOSTREAM_HANDLER2)
endif(HAVE_IOSTREAM_HANDLER2)

set(raptor_serializer_SRC
  raptorserializer.cpp
)

qt4_automoc(${raptor_serializer_SRC})

add_library(soprano_raptorserializer SHARED ${raptor_serializer_SRC})

TARGET_LINK_LIBRARIES(soprano_raptorserializer soprano soprano_redlandcore ${RAPTOR_LIBRARIES})

ADD_DEFINITIONS(${REDLAND_FLAGS})

set_target_properties(soprano_raptorserializer PROPERTIES
  DEFINE_SYMBOL MAKE_RAPTORSERIALIZER_LIB
)

INSTALL(TARGETS soprano_raptorserializer
	LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
	RUNTIME DESTINATION bin/soprano
	ARCHIVE DESTINATION lib/soprano
)

configure_file(raptorserializer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/raptorserializer.desktop
  DESTINATION share/soprano/plugins
  )
