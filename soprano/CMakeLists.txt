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
    Model.cpp
    ModelFactory.cpp
    Parser.cpp
    Manager.cpp

    # RedlandBackend
    RedlandUtil.cpp
    World.cpp
    RedlandStatementIterator.cpp
    RedlandQueryResult.cpp
    RedlandModel.cpp
    RedlandModelFactory.cpp
    RedlandParser.cpp 
   )    

if (KNEP_FOUND)
   SET(nepomuk_SRCS
      NepomukUtil.cpp
      NepomukModel.cpp
      NepomukStatementIterator.cpp
      NepomukModelFactory.cpp
   )
endif(KNEP_FOUND)

ADD_DEFINITIONS(${REDLAND_CFLAGS})
ADD_LIBRARY(soprano SHARED ${soprano_SRCS} ${nepomuk_SRCS})

TARGET_LINK_LIBRARIES(soprano ${REDLAND_LIBRARIES})
TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})
if (KNEP_FOUND)
TARGET_LINK_LIBRARIES(soprano ${KNEP_LIBRARIES})
INCLUDE_DIRECTORIES(${KNEPCLIENT_INCLUDE_DIR})
endif (KNEP_FOUND)

set_target_properties(soprano PROPERTIES VERSION 1.0.0 SOVERSION 1)

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
	ModelFactory.h
	Manager.h
	Soprano.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/soprano)
