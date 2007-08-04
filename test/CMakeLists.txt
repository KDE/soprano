project(soprano_tests)

include_directories(
  ${soprano_SOURCE_DIR}
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}
)

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

# RedlandStatementIterator
qt4_automoc(RedlandStatementIteratorTest.cpp)
add_executable(redlandstitest StatementIteratorTest.cpp RedlandStatementIteratorTest.cpp)
target_link_libraries(redlandstitest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(redlandstitest ${EXECUTABLE_OUTPUT_PATH}/redlandstitest)

# general model test
qt4_automoc(sopranotest SopranoModelTest.cpp)
add_library(sopranomodeltest STATIC SopranoModelTest.cpp)
target_link_libraries(sopranomodeltest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

# redland model tests
qt4_automoc(sopranommtest RedlandMemoryModelTest.cpp)
add_executable(sopranommtest RedlandMemoryModelTest.cpp)
target_link_libraries(sopranommtest sopranomodeltest)
add_test(sopranommtest ${EXECUTABLE_OUTPUT_PATH}/sopranommtest)

qt4_automoc(sopranopmtest RedlandPersistentModelTest.cpp)
add_executable(sopranopmtest RedlandPersistentModelTest.cpp)
target_link_libraries(sopranopmtest sopranomodeltest)
add_test(sopranopmtest ${EXECUTABLE_OUTPUT_PATH}/sopranopmtest)

# 3Store model test
qt4_automoc(tstoremodeltest 3storemodeltest.cpp)
add_executable(tstoremodeltest 3storemodeltest.cpp)
target_link_libraries(tstoremodeltest sopranomodeltest)
add_test(tstoremodeltest ${EXECUTABLE_OUTPUT_PATH}/tstoremodeltest)

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
