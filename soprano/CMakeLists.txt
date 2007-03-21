project(soprano_core)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

SET(soprano_SRCS 
    query.cpp	    
    queryresult.cpp
    resultset.cpp  
    node.cpp
    statement.cpp
    statementiterator.cpp
    statementiteratorprivate.cpp
    model.cpp
    parser.cpp
    pluginmanager.cpp
    backend.cpp
    soprano.cpp
    error.cpp
)    

qt4_automoc(${soprano_SRCS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES
        VERSION 2.0.0
        SOVERSION 2
        DEFINE_SYMBOL MAKE_SOPRANO_LIB
)

INSTALL(TARGETS soprano
	LIBRARY DESTINATION lib
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
	DESTINATION include/soprano)

add_subdirectory(redland)
