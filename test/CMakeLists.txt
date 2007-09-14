project(soprano_tests)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_SOURCE_DIR}/data
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${QT_INCLUDES}
  ${CLUCENE_INCLUDE_DIR}
  ${CLUCENE_LIBRARY_DIR}
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

# StatementIterator
qt4_automoc(StatementIteratorTest.cpp)

# general model tests
qt4_automoc(sopranotest SopranoModelTest.cpp)
add_library(sopranomodeltest STATIC SopranoModelTest.cpp)
target_link_libraries(sopranomodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

qt4_automoc(multithreadingtest multithreadingtest.cpp)
add_library(multithreadingtest STATIC multithreadingtest.cpp)
target_link_libraries(multithreadingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

if(REDLAND_FOUND)
  # RedlandStatementIterator
  qt4_automoc(RedlandStatementIteratorTest.cpp)
  add_executable(redlandstitest StatementIteratorTest.cpp RedlandStatementIteratorTest.cpp)
  target_link_libraries(redlandstitest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(redlandstitest ${EXECUTABLE_OUTPUT_PATH}/redlandstitest)

  # redland model tests
  qt4_automoc(sopranommtest RedlandMemoryModelTest.cpp)
  add_executable(sopranommtest RedlandMemoryModelTest.cpp)
  target_link_libraries(sopranommtest sopranomodeltest)
  add_test(sopranommtest ${EXECUTABLE_OUTPUT_PATH}/sopranommtest)

  qt4_automoc(sopranopmtest RedlandPersistentModelTest.cpp)
  add_executable(sopranopmtest RedlandPersistentModelTest.cpp)
  target_link_libraries(sopranopmtest sopranomodeltest)
  add_test(sopranopmtest ${EXECUTABLE_OUTPUT_PATH}/sopranopmtest)

  qt4_automoc(redlandmultithreadtest redlandmultithreadtest.cpp)
  add_executable(redlandmultithreadtest redlandmultithreadtest.cpp)
  target_link_libraries(redlandmultithreadtest soprano multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(redlandmultithreadtest ${EXECUTABLE_OUTPUT_PATH}/redlandmultithreadtest)
endif(REDLAND_FOUND)

if(TSTORE_FOUND)
  # 3Store model test
  qt4_automoc(tstoremodeltest 3storemodeltest.cpp)
  add_executable(tstoremodeltest 3storemodeltest.cpp)
  target_link_libraries(tstoremodeltest sopranomodeltest)
  add_test(tstoremodeltest ${EXECUTABLE_OUTPUT_PATH}/tstoremodeltest)
endif(TSTORE_FOUND)

# scaling
qt4_automoc(storagescalingtest storagescalingtest.cpp)
add_executable(storagescalingtest storagescalingtest.cpp)
target_link_libraries(storagescalingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/storagescalingtest)

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
qt4_automoc(queryapitest queryapitest.cpp)
add_executable(queryapitest queryapitest.cpp)
target_link_libraries(queryapitest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(queryapitest ${EXECUTABLE_OUTPUT_PATH}/queryapitest)

# WString test
qt4_automoc(wstringtest wstringtest.cpp)
add_executable(wstringtest wstringtest.cpp ../index/wstring.cpp)
target_link_libraries(wstringtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(wstringtest ${EXECUTABLE_OUTPUT_PATH}/wstringtest)

if(JNI_1_6_FOUND)
  # Sesame2 model tests
  qt4_automoc(sesame2backendtest sesame2backendtest.cpp)
  add_executable(sesame2backendtest sesame2backendtest.cpp)
  target_link_libraries(sesame2backendtest soprano sopranomodeltest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(sesame2backendtest ${EXECUTABLE_OUTPUT_PATH}/sesame2backendtest)

  qt4_automoc(sesame2multithreadtest sesame2multithreadtest.cpp)
  add_executable(sesame2multithreadtest sesame2multithreadtest.cpp)
  target_link_libraries(sesame2multithreadtest soprano multithreadingtest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
  add_test(sesame2multithreadtest ${EXECUTABLE_OUTPUT_PATH}/sesame2multithreadtest)
endif(JNI_1_6_FOUND)

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
if(SOPRANO_BUILD_INDEX_LIB)
  qt4_automoc(cluceneindextest cluceneindextest.cpp)
  add_executable(cluceneindextest cluceneindextest.cpp)
  target_link_libraries(cluceneindextest soprano sopranoindex ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${CLUCENE_LIBRARY})
  add_test(cluceneindextest ${EXECUTABLE_OUTPUT_PATH}/cluceneindextest)
endif(SOPRANO_BUILD_INDEX_LIB)

# Server test
qt4_automoc(sopranodclienttest sopranodclienttest.cpp)
add_executable(sopranodclienttest sopranodclienttest.cpp)
target_link_libraries(sopranodclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodclienttest)

qt4_automoc(sopranodsocketclienttest sopranodsocketclienttest.cpp)
add_executable(sopranodsocketclienttest sopranodsocketclienttest.cpp)
target_link_libraries(sopranodsocketclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodsocketclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodclienttest)

qt4_automoc(sopranodbusclienttest sopranodbusclienttest.cpp)
add_executable(sopranodbusclienttest sopranodbusclienttest.cpp)
target_link_libraries(sopranodbusclienttest sopranomodeltest sopranoclient ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})
add_test(sopranodbusclienttest ${EXECUTABLE_OUTPUT_PATH}/sopranodclienttest)
