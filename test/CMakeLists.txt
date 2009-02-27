project(soprano_tests)

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
  ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(config-testdata.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-testdata.h)

# Node
qt4_automoc(nodetest NodeTest.cpp)
add_executable(nodetest NodeTest.cpp)
target_link_libraries(nodetest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/nodetest)

# Literals
qt4_automoc(literaltest sopranoliteraltest.cpp)
add_executable(literaltest sopranoliteraltest.cpp)
target_link_libraries(literaltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(literaltest ${EXECUTABLE_OUTPUT_PATH}/literaltest)

# Statement
qt4_automoc(statementtest StatementTest.cpp)
add_executable(statementtest StatementTest.cpp)
target_link_libraries(statementtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(statementtest ${EXECUTABLE_OUTPUT_PATH}/statementtest)

# Generic StatementIterator test base
qt4_automoc(StatementIteratorTest.cpp)
add_library(statementiteratortest STATIC StatementIteratorTest.cpp)
target_link_libraries(statementiteratortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

# general model tests
qt4_automoc(SopranoModelTest.cpp)
add_library(sopranomodeltest STATIC SopranoModelTest.cpp stringpool.cpp)
target_link_libraries(sopranomodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

qt4_automoc(multithreadingtest multithreadingtest.cpp)
add_library(multithreadingtest STATIC multithreadingtest.cpp)
target_link_libraries(multithreadingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

if(BUILD_REDLAND_BACKEND)
  # RedlandStatementIterator
  qt4_automoc(redlandstatementiteratortest.cpp)
  add_executable(redlandstatementiteratortest redlandstatementiteratortest.cpp)
  target_link_libraries(redlandstatementiteratortest statementiteratortest)
  add_test(redlandstatementiteratortest ${EXECUTABLE_OUTPUT_PATH}/redlandstatementiteratortest)

  # redland model tests
  qt4_automoc(redlandmemorymodeltest RedlandMemoryModelTest.cpp)
  add_executable(redlandmemorymodeltest RedlandMemoryModelTest.cpp)
  target_link_libraries(redlandmemorymodeltest sopranomodeltest)
  add_test(redlandmemorymodeltest ${EXECUTABLE_OUTPUT_PATH}/redlandmemorymodeltest)

  qt4_automoc(redlandpersistentmodeltest RedlandPersistentModelTest.cpp)
  add_executable(redlandpersistentmodeltest RedlandPersistentModelTest.cpp)
  target_link_libraries(redlandpersistentmodeltest sopranomodeltest)
  add_test(redlandpersistentmodeltest ${EXECUTABLE_OUTPUT_PATH}/redlandpersistentmodeltest)

  qt4_automoc(redlandmultithreadtest redlandmultithreadtest.cpp)
  add_executable(redlandmultithreadtest redlandmultithreadtest.cpp)
  target_link_libraries(redlandmultithreadtest soprano multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(redlandmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/redlandmultithreadtest)
endif(BUILD_REDLAND_BACKEND)

# scaling
qt4_automoc(storagescalingtest storagescalingtest.cpp)
add_executable(storagescalingtest storagescalingtest.cpp)
target_link_libraries(storagescalingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
#add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/storagescalingtest)

# SimpleStatementIterator
qt4_automoc(simplestatementiteratortest simplestatementiteratortest.cpp)
add_executable(simplestatementiteratortest simplestatementiteratortest.cpp)
target_link_libraries(simplestatementiteratortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(simplestatementiteratortest ${EXECUTABLE_OUTPUT_PATH}/simplestatementiteratortest)

# QueryResultStatementIterator
qt4_automoc(queryresultstatementiteratortest queryresultstatementiteratortest.cpp)
add_executable(queryresultstatementiteratortest queryresultstatementiteratortest.cpp)
target_link_libraries(queryresultstatementiteratortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(queryresultstatementiteratortest ${EXECUTABLE_OUTPUT_PATH}/queryresultstatementiteratortest)

# InferenceModel
qt4_automoc(inferencemodeltest inferencemodeltest.cpp)
add_executable(inferencemodeltest inferencemodeltest.cpp)
target_link_libraries(inferencemodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(inferencemodeltest ${EXECUTABLE_OUTPUT_PATH}/inferencemodeltest)

# QueryAPITest
#qt4_automoc(queryapitest queryapitest.cpp)
#add_executable(queryapitest queryapitest.cpp)
#target_link_libraries(queryapitest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
#add_test(queryapitest ${EXECUTABLE_OUTPUT_PATH}/queryapitest)

if(BUILD_SESAME2_BACKEND)
  # Sesame2 model tests
  qt4_automoc(sesame2backendtest sesame2backendtest.cpp)
  add_executable(sesame2backendtest sesame2backendtest.cpp)
  target_link_libraries(sesame2backendtest soprano sopranomodeltest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(sesame2backendtest ${EXECUTABLE_OUTPUT_PATH}/sesame2backendtest)

  qt4_automoc(sesame2multithreadtest sesame2multithreadtest.cpp)
  add_executable(sesame2multithreadtest sesame2multithreadtest.cpp)
  target_link_libraries(sesame2multithreadtest soprano multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(sesame2multithreadtest ${EXECUTABLE_OUTPUT_PATH}/sesame2multithreadtest)
endif(BUILD_SESAME2_BACKEND)

# parser test
qt4_automoc(parsertest parsertest.cpp)
add_executable(parsertest parsertest.cpp)
target_link_libraries(parsertest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(parsertest ${EXECUTABLE_OUTPUT_PATH}/parsertest)

# Error test
qt4_automoc(errortest errortest.cpp)
add_executable(errortest errortest.cpp)
target_link_libraries(errortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(errortest ${EXECUTABLE_OUTPUT_PATH}/errortest)

# Index Test
if(BUILD_CLUCENE_INDEX)
  include_directories(
    ${CLUCENE_INCLUDE_DIR}
    ${CLUCENE_LIBRARY_DIR})

  # TString test
  qt4_automoc(tstringtest tstringtest.cpp)
  add_executable(tstringtest tstringtest.cpp stringpool.cpp ../index/tstring.cpp)
  target_link_libraries(tstringtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${CLUCENE_LIBRARY})
  add_test(tstringtest ${EXECUTABLE_OUTPUT_PATH}/tstringtest)

  qt4_automoc(cluceneindextest cluceneindextest.cpp)
  add_executable(cluceneindextest cluceneindextest.cpp stringpool.cpp)
  target_link_libraries(cluceneindextest soprano sopranoindex ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${CLUCENE_LIBRARY})
  add_test(cluceneindextest ${EXECUTABLE_OUTPUT_PATH}/cluceneindextest)

  qt4_automoc(indexmodelmultithreadingtest indexmodelmultithreadingtest.cpp)
  add_executable(indexmodelmultithreadingtest indexmodelmultithreadingtest.cpp)
  target_link_libraries(indexmodelmultithreadingtest soprano sopranoindex multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(indexmodelmultithreadingtest ${EXECUTABLE_OUTPUT_PATH}/indexmodelmultithreadingtest)
endif(BUILD_CLUCENE_INDEX)

# Server test
qt4_automoc(sopranodclienttest sopranodclienttest.cpp)
add_executable(sopranodclienttest sopranodclienttest.cpp)
target_link_libraries(sopranodclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodclienttest)

# LocalSocketClient multithreading test
qt4_automoc(localsocketmultithreadtest localsocketmultithreadingtest.cpp)
add_executable(localsocketmultithreadtest localsocketmultithreadingtest.cpp)
target_link_libraries(localsocketmultithreadtest soprano sopranoclient multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(localsocketmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/localsocketmultithreadtest)

qt4_automoc(sopranodsocketclienttest sopranodsocketclienttest.cpp)
add_executable(sopranodsocketclienttest sopranodsocketclienttest.cpp)
target_link_libraries(sopranodsocketclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodsocketclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodclienttest)

if(BUILD_DBUS_SUPPORT)
qt4_automoc(sopranodbusclienttest sopranodbusclienttest.cpp)
add_executable(sopranodbusclienttest sopranodbusclienttest.cpp)
target_link_libraries(sopranodbusclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodbusclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodclienttest)

qt4_automoc(sopranodbusmultithreadtest sopranodbusmultithreadtest.cpp)
add_executable(sopranodbusmultithreadtest sopranodbusmultithreadtest.cpp)
target_link_libraries(sopranodbusmultithreadtest soprano sopranoclient multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(sopranodbusmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/sopranodbusmultithreadtest)

# Signal spy
qt4_automoc(sopranodbussignalspy modelsignalspy.cpp)
add_executable(sopranodbussignalspy modelsignalspy.cpp sopranodbussignalspy.cpp)
target_link_libraries(sopranodbussignalspy sopranoclient ${QT_QTCORE_LIBRARY})
endif(BUILD_DBUS_SUPPORT)

# NRL Model test
qt4_automoc(nrlmodeltest nrlmodeltest.cpp)
add_executable(nrlmodeltest nrlmodeltest.cpp)
target_link_libraries(nrlmodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nrlmodeltest ${EXECUTABLE_OUTPUT_PATH}/nrlmodeltest)

# Server QDataStream operators
qt4_automoc(serveroperatortest.cpp)
add_executable(serveroperatortest serveroperatortest.cpp ../server/datastream.cpp)
target_link_libraries(serveroperatortest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

# async model test
set(asyncmodeltest_SRC asyncresultwaiter.cpp asyncmodeltest.cpp)
qt4_automoc(${asyncmodeltest_SRC})
add_executable(asyncmodeltest ${asyncmodeltest_SRC})
target_link_libraries(asyncmodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
