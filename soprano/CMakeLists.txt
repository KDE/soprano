project(soprano_core)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/soprano
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

SET(soprano_SRCS
    query.cpp
    query.h
    queryresult.cpp
    queryresult.h
    resultset.cpp
    resultset.h
    node.cpp
    node.h
    statement.cpp
    statement.h
    statementiterator.cpp
    statementiterator.h
    statementiteratorprivate.cpp
    statementiteratorprivate.h
    model.cpp
    model.h
    parser.cpp
    parser.h
    pluginmanager.cpp
    pluginmanager.h
    backend.cpp
    backend.h
    soprano.cpp
    soprano.h
    error.cpp
    error.h
    literalvalue.cpp
    literalvalue.h
    datetime.cpp
    version.cpp
)

qt4_automoc(${soprano_SRCS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES
        VERSION 3.0.0
        SOVERSION 3
        DEFINE_SYMBOL MAKE_SOPRANO_LIB
)

INSTALL(TARGETS soprano
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)

INSTALL(FILES 
	parser.h
	query.h
	resultset.h
	node.h
	statement.h
	statementiterator.h
	model.h
	pluginmanager.h
	soprano.h
	soprano_export.h
	backend.h
	error.h
	literalvalue.h
	version.h
	DESTINATION include/soprano)

add_subdirectory(redland)
