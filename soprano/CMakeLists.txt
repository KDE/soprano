include(KDE4Defaults)

SET(soprano_SRCS 
    Query.cpp	    
    QueryResult.cpp  
    QueryResultIterator.cpp
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

    # NepomukBackend
    NepomukUtil.cpp
    NepomukModel.cpp
    NepomukStatementIterator.cpp
    NepomukModelFactory.cpp
   )    

ADD_DEFINITIONS(${REDLAND_CFLAGS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${RAPTOR_LIBRARIES})
TARGET_LINK_LIBRARIES(soprano ${RASQAL_LIBRARIES})
TARGET_LINK_LIBRARIES(soprano ${REDLAND_LIBRARIES})
TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})
TARGET_LINK_LIBRARIES(soprano knepclient)

INSTALL(TARGETS soprano
		LIBRARY DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES 
	Parser.h
	Query.h
	QueryResult.h
	QueryResultIterator.h
	Node.h
	Statement.h
	StatementIterator.h
	Model.h
	ModelFactory.h
	Manager.h
	Soprano.h
	DESTINATION include/soprano)
