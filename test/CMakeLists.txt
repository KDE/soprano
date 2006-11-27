include(KDE4Defaults)
include(MacroLibrary)
include_directories(${QT_INCLUDES})

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
