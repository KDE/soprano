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

# general
qt4_automoc(sopranotest SopranoModelTest.cpp)
qt4_automoc(sopranommtest RedlandMemoryModelTest.cpp)
qt4_automoc(sopranopmtest RedlandPersistentModelTest.cpp)

add_executable(sopranommtest SopranoModelTest.cpp RedlandMemoryModelTest.cpp)
target_link_libraries(sopranommtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(sopranommtest ${EXECUTABLE_OUTPUT_PATH}/sopranommtest)

add_executable(sopranopmtest SopranoModelTest.cpp RedlandPersistentModelTest.cpp)
target_link_libraries(sopranopmtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(sopranopmtest ${EXECUTABLE_OUTPUT_PATH}/sopranopmtest)

# scaling
qt4_automoc(storagescalingtest storagescalingtest.cpp)
add_executable(storagescalingtest storagescalingtest.cpp)
target_link_libraries(storagescalingtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/storagescalingtest)
