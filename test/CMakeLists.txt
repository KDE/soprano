include(KDE4Defaults)
include(MacroLibrary)
include_directories(${CMAKE_SOURCE_DIR} ${QT_INCLUDES})

# Node
kde4_automoc(nodetest NodeTest.cpp)
kde4_add_executable(nodetest NodeTest.cpp)
target_link_libraries(nodetest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(nodetest ${EXECUTABLE_OUTPUT_PATH}/nodetest)

# Statement
kde4_automoc(statementtest StatementTest.cpp)
kde4_add_executable(statementtest StatementTest.cpp)
target_link_libraries(statementtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(statementtest ${EXECUTABLE_OUTPUT_PATH}/statementtest)

# StatementIterator
kde4_automoc(StatementIteratorTest.cpp)

# RedlandStatementIterator
kde4_automoc(RedlandStatementIteratorTest.cpp)
kde4_add_executable(redlandstitest StatementIteratorTest.cpp RedlandStatementIteratorTest.cpp)
target_link_libraries(redlandstitest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(redlandstitest ${EXECUTABLE_OUTPUT_PATH}/redlandstitest)

# general
kde4_automoc(sopranotest SopranoModelTest.cpp)
kde4_automoc(sopranommtest RedlandMemoryModelTest.cpp)
kde4_automoc(sopranopmtest RedlandPersistentModelTest.cpp)

kde4_add_executable(sopranommtest SopranoModelTest.cpp RedlandMemoryModelTest.cpp)
target_link_libraries(sopranommtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(sopranommtest ${EXECUTABLE_OUTPUT_PATH}/sopranommtest)

kde4_add_executable(sopranopmtest SopranoModelTest.cpp RedlandPersistentModelTest.cpp)
target_link_libraries(sopranopmtest soprano ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(sopranopmtest ${EXECUTABLE_OUTPUT_PATH}/sopranopmtest)

if (KNEP_FOUND)
   kde4_automoc(sopranonmtest NepomukModelTest.cpp)
   kde4_add_executable(sopranonmtest SopranoModelTest.cpp NepomukModelTest.cpp)
   target_link_libraries(sopranonmtest soprano ${KNEP_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
   add_test(sopranonmtest ${EXECUTABLE_OUTPUT_PATH}/sopranonmtest)
endif (KNEP_FOUND)
