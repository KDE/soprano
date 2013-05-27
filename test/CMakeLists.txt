project(soprano_tests)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# QTEST_MAIN is using QApplication when QT_GUI_LIB is defined
remove_definitions(-DQT_GUI_LIB)

if(QT5_BUILD)
  find_package(Qt5Test CONFIG REQUIRED)
  include_directories(${Qt5Test_INCLUDE_DIRS})
  set(Soprano_test_link_libraries ${Qt5Core_LIBRARIES} ${Qt5Test_LIBRARIES})
else()
  set(Soprano_test_link_libraries ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
endif()

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_BINARY_DIR}
  ${soprano_SOURCE_DIR}/data
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_BINARY_DIR}
)

configure_file(config-testdata.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-testdata.h)

# DateTime
add_executable(datetimetest datetimetest.cpp ../soprano/datetime.cpp)
target_link_libraries(datetimetest ${Soprano_test_link_libraries})
add_test(datetimetest datetimetest)

# Node
add_executable(nodetest NodeTest.cpp)
target_link_libraries(nodetest soprano ${Soprano_test_link_libraries})
add_test(nodetest nodetest)

# N3 Node parser
add_executable(n3nodeparsertest n3nodeparsertest.cpp)
target_link_libraries(n3nodeparsertest soprano ${Soprano_test_link_libraries})
add_test(n3nodeparsertest n3nodeparsertest)

# Literals
add_executable(literaltest sopranoliteraltest.cpp)
target_link_libraries(literaltest soprano ${Soprano_test_link_libraries})
add_test(literaltest literaltest)

# Statement
add_executable(statementtest StatementTest.cpp)
target_link_libraries(statementtest soprano ${Soprano_test_link_libraries})
add_test(statementtest statementtest)

# Generic StatementIterator test base
add_library(statementiteratortest STATIC StatementIteratorTest.cpp)
target_link_libraries(statementiteratortest soprano ${Soprano_test_link_libraries})

# general model tests
add_library(sopranomodeltest STATIC SopranoModelTest.cpp stringpool.cpp)
target_link_libraries(sopranomodeltest soprano ${Soprano_test_link_libraries})

add_library(multithreadingtest STATIC multithreadingtest.cpp)
target_link_libraries(multithreadingtest soprano ${Soprano_test_link_libraries})

if(BUILD_REDLAND_BACKEND)
  # RedlandStatementIterator
  add_executable(redlandstatementiteratortest redlandstatementiteratortest.cpp)
  target_link_libraries(redlandstatementiteratortest statementiteratortest)
  add_test(redlandstatementiteratortest redlandstatementiteratortest)

  # redland model tests
  add_executable(redlandmemorymodeltest RedlandMemoryModelTest.cpp)
  target_link_libraries(redlandmemorymodeltest sopranomodeltest)
  add_test(redlandmemorymodeltest redlandmemorymodeltest)

  add_executable(redlandpersistentmodeltest RedlandPersistentModelTest.cpp)
  target_link_libraries(redlandpersistentmodeltest sopranomodeltest)
  add_test(redlandpersistentmodeltest redlandpersistentmodeltest)

  add_executable(redlandmultithreadtest redlandmultithreadtest.cpp)
  target_link_libraries(redlandmultithreadtest soprano multithreadingtest ${Soprano_test_link_libraries})
  add_test(redlandmultithreadtest redlandmultithreadtest)
endif()

# scaling
add_executable(storagescalingtest storagescalingtest.cpp)
target_link_libraries(storagescalingtest soprano ${Soprano_test_link_libraries})
#add_test(nodetest storagescalingtest)

# SimpleStatementIterator
add_executable(simplestatementiteratortest simplestatementiteratortest.cpp)
target_link_libraries(simplestatementiteratortest soprano ${Soprano_test_link_libraries})
add_test(simplestatementiteratortest simplestatementiteratortest)

# QueryResultStatementIterator
add_executable(queryresultstatementiteratortest queryresultstatementiteratortest.cpp)
target_link_libraries(queryresultstatementiteratortest soprano ${Soprano_test_link_libraries})
add_test(queryresultstatementiteratortest queryresultstatementiteratortest)

# InferenceModel
add_executable(inferencemodeltest inferencemodeltest.cpp)
target_link_libraries(inferencemodeltest soprano ${Soprano_test_link_libraries})
add_test(inferencemodeltest inferencemodeltest)

if(BUILD_SESAME2_BACKEND)
  # Sesame2 model tests
  add_executable(sesame2backendtest sesame2backendtest.cpp)
  target_link_libraries(sesame2backendtest soprano sopranomodeltest ${Soprano_test_link_libraries})
  add_test(sesame2backendtest sesame2backendtest)

  add_executable(sesame2multithreadtest sesame2multithreadtest.cpp)
  target_link_libraries(sesame2multithreadtest soprano multithreadingtest ${Soprano_test_link_libraries})
  add_test(sesame2multithreadtest sesame2multithreadtest)
endif()

# parser test
add_executable(parsertest parsertest.cpp)
target_link_libraries(parsertest soprano ${Soprano_test_link_libraries})
add_test(parsertest parsertest)

# serializer test
add_executable(serializertest serializetest.cpp)
target_link_libraries(serializertest soprano ${Soprano_test_link_libraries})
add_test(serializertest serializertest)

# Error test
add_executable(errortest errortest.cpp)
target_link_libraries(errortest soprano ${Soprano_test_link_libraries})
add_test(errortest errortest)

# Index Test
if(BUILD_CLUCENE_INDEX)
  include_directories(
    ${CLUCENE_INCLUDE_DIR}
    ${CLUCENE_LIBRARY_DIR})

  # TString test
  add_executable(tstringtest tstringtest.cpp stringpool.cpp ../index/tstring.cpp)
  target_link_libraries(tstringtest ${Soprano_test_link_libraries} ${CLUCENE_LIBRARIES})
  add_test(tstringtest tstringtest)

  add_executable(cluceneindextest cluceneindextest.cpp stringpool.cpp)
  target_link_libraries(cluceneindextest soprano sopranoindex ${Soprano_test_link_libraries} ${CLUCENE_LIBRARIES})
  add_test(cluceneindextest cluceneindextest)

  add_executable(indexmodelmultithreadingtest indexmodelmultithreadingtest.cpp)
  target_link_libraries(indexmodelmultithreadingtest soprano sopranoindex multithreadingtest ${Soprano_test_link_libraries})
  add_test(indexmodelmultithreadingtest indexmodelmultithreadingtest)
endif()

if(NOT QT5_BUILD)
  # LocalSocketClient multithreading test
  add_executable(localsocketmultithreadtest localsocketmultithreadingtest.cpp)
  target_link_libraries(localsocketmultithreadtest soprano sopranoclient multithreadingtest ${Soprano_test_link_libraries})
  add_test(localsocketmultithreadtest localsocketmultithreadtest)

  add_executable(sopranodsocketclienttest sopranodsocketclienttest.cpp)
  target_link_libraries(sopranodsocketclienttest sopranomodeltest sopranoclient ${Soprano_test_link_libraries} ${QT_QTNETWORK_LIBRARY})
  add_test(sopranodsocketclienttest sopranodsocketclienttest)

  if(BUILD_DBUS_SUPPORT)
    add_executable(sopranodbusclienttest sopranodbusclienttest.cpp)
    target_link_libraries(sopranodbusclienttest sopranomodeltest sopranoclient ${Soprano_test_link_libraries} ${QT_QTNETWORK_LIBRARY})
    add_test(sopranodbusclienttest sopranodbusclienttest)

    add_executable(sopranodbusmultithreadtest sopranodbusmultithreadtest.cpp)
    target_link_libraries(sopranodbusmultithreadtest soprano sopranoclient multithreadingtest ${Soprano_test_link_libraries})
    add_test(sopranodbusmultithreadtest sopranodbusmultithreadtest)

    # Signal spy
    add_executable(sopranodbussignalspy modelsignalspy.cpp sopranodbussignalspy.cpp)
    target_link_libraries(sopranodbussignalspy sopranoclient ${QT_QTCORE_LIBRARY})
  endif()
endif()

# NRL Model test
add_executable(nrlmodeltest nrlmodeltest.cpp)
target_link_libraries(nrlmodeltest soprano ${Soprano_test_link_libraries})
add_test(nrlmodeltest nrlmodeltest)

# Server QDataStream operators
add_executable(serveroperatortest serveroperatortest.cpp ../server/serverdatastream.cpp)
target_link_libraries(serveroperatortest soprano ${Soprano_test_link_libraries})

# async model test
set(asyncmodeltest_SRC asyncresultwaiter.cpp asyncmodeltest.cpp)
add_executable(asyncmodeltest ${asyncmodeltest_SRC})
target_link_libraries(asyncmodeltest soprano ${Soprano_test_link_libraries})

if(BUILD_VIRTUOSO_BACKEND)
  add_executable(virtuosobackendtest virtuosobackendtest.cpp)
  target_link_libraries(virtuosobackendtest sopranomodeltest)
  add_test(virtuosobackendtest virtuosobackendtest)
endif()

# graph model tests
add_executable(graphtest graphtest.cpp)
target_link_libraries(graphtest sopranomodeltest)
add_test(graphtest graphtest)

# async query test
add_executable(asyncquerytest asyncquerytest.cpp)
target_link_libraries(asyncquerytest soprano ${Soprano_test_link_libraries})
add_test(asyncquerytest asyncquerytest)
