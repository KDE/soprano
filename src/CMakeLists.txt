
SET(qrdf_SRCS 
    Query.cpp	    
    QueryResult.cpp  
    Node.cpp
    Statement.cpp
    Model.cpp
    ModelFactory.cpp
    Parser.cpp

    World.cpp
    RedlandUtil.cpp
    RedlandQueryResult.cpp
    RedlandModel.cpp
    RedlandModelFactory.cpp
    RedlandParser.cpp)    

ADD_DEFINITIONS(${REDLAND_CFLAGS})

ADD_LIBRARY(qrdf SHARED ${qrdf_SRCS})

INCLUDE_DIRECTORIES(${QT_QTCORE_INCLUDE_DIR} ${QT_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(qrdf ${RAPTOR_LIBRARIES})
TARGET_LINK_LIBRARIES(qrdf ${RASQAL_LIBRARIES})
TARGET_LINK_LIBRARIES(qrdf ${REDLAND_LIBRARIES})
TARGET_LINK_LIBRARIES(qrdf ${QT_QTCORE_LIBRARY})

INSTALL(TARGETS qrdf
		LIBRARY DESTINATION lib)

INSTALL(FILES Model.h Parser.h
	DESTINATION include)