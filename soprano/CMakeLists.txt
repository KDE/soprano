project(soprano_core)

if (WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif (WIN32)
 
if (APPLE)
 set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
 set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (APPLE)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${CMAKE_SOURCE_DIR}/soprano
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

SET(soprano_SRCS
    query.cpp
    query.h
    queryresultiterator.cpp
    queryresultiterator.h
    queryresultiteratorbackend.cpp
    queryresultiteratorbackend.h
    node.cpp
    node.h
    statement.cpp
    statement.h
    statementiterator.cpp
    statementiterator.h
    statementiteratorbackend.cpp
    statementiteratorbackend.h
    simplestatementiterator.cpp
    queryresultstatementiterator.cpp
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
    filtermodel.h
    filtermodel.cpp
    storagemodel.h
    storagemodel.cpp
    rdfschemawrapper.cpp
    simplestatementiterator.h
    vocabulary/rdf.cpp
    vocabulary/rdfs.cpp
    vocabulary/xmls.cpp
    inference/nodepattern.cpp
    inference/statementpattern.cpp
    inference/inferencerule.cpp
    inference/sil.cpp
    inference/inferencemodel.h
    inference/inferencemodel.cpp
)

qt4_automoc(${soprano_SRCS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES
        VERSION 4.0.0
        SOVERSION 4
        DEFINE_SYMBOL MAKE_SOPRANO_LIB
	INSTALL_NAME_DIR ${LIB_DESTINATION}
)

INSTALL(TARGETS soprano
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)

INSTALL(FILES 
	parser.h
	query.h
	queryresultiterator.h
	queryresultiteratorbackend.h
	node.h
	statement.h
	statementiterator.h
	statementiteratorbackend.h
	simplestatementiterator.h
	queryresultstatementiterator.h
	model.h
	filtermodel.h
	storagemodel.h
	pluginmanager.h
	soprano.h
	soprano_export.h
	backend.h
	error.h
	literalvalue.h
	version.h
	rdfschemawrapper.h
	vocabulary/rdf.h
	vocabulary/rdfs.h
	vocabulary/xmls.h
	inference/inferencemodel.h
	inference/inferencerule.h
	inference/nodepattern.h
	inference/statementpattern.h
	inference/sil.h
	DESTINATION include/soprano)

add_subdirectory(backends)

find_package(CLucene)
if(CLucene_FOUND)
  add_subdirectory(index)
endif(CLucene_FOUND)

#add_subdirectory(query)
