project(soprano_core)

include(KDE4Defaults)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)

SET(soprano_SRCS 
    Query.cpp	    
    QueryResult.cpp
    ResultSet.cpp  
    Node.cpp
    Statement.cpp
    StatementIterator.cpp
    StatementIteratorPrivate.cpp
    Model.cpp
    Parser.cpp
    pluginmanager.cpp
    backend.cpp
    soprano.cpp
)    

qt4_automoc(${soprano_SRCS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES VERSION 2.0.0 SOVERSION 2)

INSTALL(TARGETS soprano
		LIBRARY DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES 
	Parser.h
	Query.h
	ResultSet.h
	Node.h
	Statement.h
	StatementIterator.h
	Model.h
	pluginmanager.h
	Soprano.h
	soprano_export.h
	backend.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/soprano)

add_subdirectory(redland)
