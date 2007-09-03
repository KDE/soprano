project(soprano) 

enable_testing()
 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules) 

find_package(GNUWIN32) 
find_package(Qt4 REQUIRED) 

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

add_subdirectory(soprano) 
add_subdirectory(backends)
add_subdirectory(parsers)
add_subdirectory(serializers)
add_subdirectory(queryparsers)
add_subdirectory(server)
add_subdirectory(tools)
add_subdirectory(test) 
add_subdirectory(rules)
add_subdirectory(includes)

# apidox
find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  configure_file(Doxyfile.cmake Doxyfile)

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "${QT_DOC_DIR}/html")
  else(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "http://doc.trolltech.com/4.2/")
  endif(EXISTS ${QT_DOC_DIR}/html)

  add_custom_target(
    apidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND docs/html/installdox -l qt4.tag@${QTDOCS} docs/html/*.html)
endif(DOXYGEN_EXECUTABLE)
