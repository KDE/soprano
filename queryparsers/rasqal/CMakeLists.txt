project(soprano_rasqalqueryparser)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${REDLAND_INCLUDE_DIR}
  ${RASQAL_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  ${soprano_rasqalqueryparser_BINARY_DIR}
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
)

set(rasqalqueryparser_SRC
  rasqalqueryparser.cpp
  rasqalqueryparser.h
)


add_library(soprano_rasqalqueryparser MODULE ${rasqalqueryparser_SRC})

TARGET_LINK_LIBRARIES(soprano_rasqalqueryparser soprano ${RAPTOR_LIBRARIES} ${RASQAL_LIBRARIES} ${REDLAND_LIBRARIES})

ADD_DEFINITIONS(${REDLAND_CFLAGS})

set_target_properties(soprano_rasqalqueryparser PROPERTIES
        DEFINE_SYMBOL MAKE_RASQALQUERYPARSER_LIB
)

if(WIN32)
# RUNTIME and ARCHIVE are ignored when the library type is set to MODULE (cf. add_library above)
# so specify the bin directory explicitely. Only the .dll will be copied.
  INSTALL(TARGETS soprano_rasqalqueryparser DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  INSTALL(TARGETS soprano_rasqalqueryparser
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/soprano
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}/soprano
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/soprano
    )
endif()

configure_file(rasqalqueryparser.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/rasqalqueryparser.desktop)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/rasqalqueryparser.desktop
  DESTINATION ${DATA_INSTALL_DIR}/soprano/plugins
  )
