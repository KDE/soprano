project(soprano_tests)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# QTEST_MAIN is using QApplication when QT_GUI_LIB is defined
remove_definitions(-DQT_GUI_LIB)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_BINARY_DIR}
  ${soprano_SOURCE_DIR}/data
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_BINARY_DIR}
  ${QT_INCLUDES}
)

configure_file(config-testdata.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-testdata.h)

# DateTime
add_executable(datetimetest datetimetest.cpp ../soprano/datetime.cpp)
target_link_libraries(datetimetest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(datetimetest ${EXECUTABLE_OUTPUT_PATH}/datetimetest)

# Node
add_executable(nodetest NodeTest.cpp)
target_link_libraries(nodetest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/nodetest)

# N3 Node parser
add_executable(n3nodeparsertest n3nodeparsertest.cpp)
target_link_libraries(n3nodeparsertest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(n3nodeparsertest ${EXECUTABLE_OUTPUT_PATH}/n3nodeparsertest)

# Literals
add_executable(literaltest sopranoliteraltest.cpp)
target_link_libraries(literaltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(literaltest ${EXECUTABLE_OUTPUT_PATH}/literaltest)

# Statement
add_executable(statementtest StatementTest.cpp)
target_link_libraries(statementtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(statementtest ${EXECUTABLE_OUTPUT_PATH}/statementtest)

# Generic StatementIterator test base
add_library(statementiteratortest STATIC StatementIteratorTest.cpp)
target_link_libraries(statementiteratortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

# general model tests
add_library(sopranomodeltest STATIC SopranoModelTest.cpp stringpool.cpp)
target_link_libraries(sopranomodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

add_library(multithreadingtest STATIC multithreadingtest.cpp)
target_link_libraries(multithreadingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

if(BUILD_REDLAND_BACKEND)
  # RedlandStatementIterator
  add_executable(redlandstatementiteratortest redlandstatementiteratortest.cpp)
  target_link_libraries(redlandstatementiteratortest statementiteratortest)
  add_test(redlandstatementiteratortest ${EXECUTABLE_OUTPUT_PATH}/redlandstatementiteratortest)

  # redland model tests
  add_executable(redlandmemorymodeltest RedlandMemoryModelTest.cpp)
  target_link_libraries(redlandmemorymodeltest sopranomodeltest)
  add_test(redlandmemorymodeltest ${EXECUTABLE_OUTPUT_PATH}/redlandmemorymodeltest)

  add_executable(redlandpersistentmodeltest RedlandPersistentModelTest.cpp)
  target_link_libraries(redlandpersistentmodeltest sopranomodeltest)
  add_test(redlandpersistentmodeltest ${EXECUTABLE_OUTPUT_PATH}/redlandpersistentmodeltest)

  add_executable(redlandmultithreadtest redlandmultithreadtest.cpp)
  target_link_libraries(redlandmultithreadtest soprano multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(redlandmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/redlandmultithreadtest)
endif()

# scaling
add_executable(storagescalingtest storagescalingtest.cpp)
target_link_libraries(storagescalingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
#add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/storagescalingtest)

# SimpleStatementIterator
add_executable(simplestatementiteratortest simplestatementiteratortest.cpp)
target_link_libraries(simplestatementiteratortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(simplestatementiteratortest ${EXECUTABLE_OUTPUT_PATH}/simplestatementiteratortest)

# QueryResultStatementIterator
add_executable(queryresultstatementiteratortest queryresultstatementiteratortest.cpp)
target_link_libraries(queryresultstatementiteratortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(queryresultstatementiteratortest ${EXECUTABLE_OUTPUT_PATH}/queryresultstatementiteratortest)

# InferenceModel
add_executable(inferencemodeltest inferencemodeltest.cpp)
target_link_libraries(inferencemodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(inferencemodeltest ${EXECUTABLE_OUTPUT_PATH}/inferencemodeltest)

if(BUILD_SESAME2_BACKEND)
  # Sesame2 model tests
  add_executable(sesame2backendtest sesame2backendtest.cpp)
  target_link_libraries(sesame2backendtest soprano sopranomodeltest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(sesame2backendtest ${EXECUTABLE_OUTPUT_PATH}/sesame2backendtest)

  add_executable(sesame2multithreadtest sesame2multithreadtest.cpp)
  target_link_libraries(sesame2multithreadtest soprano multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(sesame2multithreadtest ${EXECUTABLE_OUTPUT_PATH}/sesame2multithreadtest)
endif()

# parser test
add_executable(parsertest parsertest.cpp)
target_link_libraries(parsertest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(parsertest ${EXECUTABLE_OUTPUT_PATH}/parsertest)

# serializer test
add_executable(serializertest serializetest.cpp)
target_link_libraries(serializertest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(serializertest ${EXECUTABLE_OUTPUT_PATH}/serializertest)

# Error test
add_executable(errortest errortest.cpp)
target_link_libraries(errortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(errortest ${EXECUTABLE_OUTPUT_PATH}/errortest)

# Index Test
if(BUILD_CLUCENE_INDEX)
  include_directories(
    ${CLUCENE_INCLUDE_DIR}
    ${CLUCENE_LIBRARY_DIR})

  # TString test
  add_executable(tstringtest tstringtest.cpp stringpool.cpp ../index/tstring.cpp)
  target_link_libraries(tstringtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${CLUCENE_LIBRARIES})
  add_test(tstringtest ${EXECUTABLE_OUTPUT_PATH}/tstringtest)

  add_executable(cluceneindextest cluceneindextest.cpp stringpool.cpp)
  target_link_libraries(cluceneindextest soprano sopranoindex ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${CLUCENE_LIBRARIES})
  add_test(cluceneindextest ${EXECUTABLE_OUTPUT_PATH}/cluceneindextest)

  add_executable(indexmodelmultithreadingtest indexmodelmultithreadingtest.cpp)
  target_link_libraries(indexmodelmultithreadingtest soprano sopranoindex multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(indexmodelmultithreadingtest ${EXECUTABLE_OUTPUT_PATH}/indexmodelmultithreadingtest)
endif()

# LocalSocketClient multithreading test
add_executable(localsocketmultithreadtest localsocketmultithreadingtest.cpp)
target_link_libraries(localsocketmultithreadtest soprano sopranoclient multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(localsocketmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/localsocketmultithreadtest)

add_executable(sopranodsocketclienttest sopranodsocketclienttest.cpp)
target_link_libraries(sopranodsocketclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodsocketclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodsocketclienttest)

if(BUILD_DBUS_SUPPORT)
add_executable(sopranodbusclienttest sopranodbusclienttest.cpp)
target_link_libraries(sopranodbusclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodbusclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodbusclienttest)

add_executable(sopranodbusmultithreadtest sopranodbusmultithreadtest.cpp)
target_link_libraries(sopranodbusmultithreadtest soprano sopranoclient multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(sopranodbusmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/sopranodbusmultithreadtest)

# Signal spy
add_executable(sopranodbussignalspy modelsignalspy.cpp sopranodbussignalspy.cpp)
target_link_libraries(sopranodbussignalspy sopranoclient ${QT_QTCORE_LIBRARY})
endif()

# NRL Model test
add_executable(nrlmodeltest nrlmodeltest.cpp)
target_link_libraries(nrlmodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nrlmodeltest ${EXECUTABLE_OUTPUT_PATH}/nrlmodeltest)

# Server QDataStream operators
add_executable(serveroperatortest serveroperatortest.cpp ../server/serverdatastream.cpp)
target_link_libraries(serveroperatortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

# async model test
set(asyncmodeltest_SRC asyncresultwaiter.cpp asyncmodeltest.cpp)
add_executable(asyncmodeltest ${asyncmodeltest_SRC})
target_link_libraries(asyncmodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

if(BUILD_VIRTUOSO_BACKEND)
  add_executable(virtuosobackendtest virtuosobackendtest.cpp)
  target_link_libraries(virtuosobackendtest sopranomodeltest)
  add_test(virtuosobackendtest virtuosobackendtest)
endif()

# graph model tests
add_executable(graphtest graphtest.cpp)
target_link_libraries(graphtest sopranomodeltest)
add_test(graphtest ${EXECUTABLE_OUTPUT_PATH}/graphtest)

# async query test
add_executable(asyncquerytest asyncquerytest.cpp)
target_link_libraries(asyncquerytest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(asyncquerytest ${EXECUTABLE_OUTPUT_PATH}/asyncquerytest)
