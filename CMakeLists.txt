project(Soprano) 

enable_testing()
 
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules) 
set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib ) 

find_package(KDE4 REQUIRED) 
find_package(Qt4 REQUIRED) 
find_package(Redland REQUIRED) 
find_package(KNep REQUIRED) 
 
if(QT4_FOUND AND KDE4_FOUND AND Redland_FOUND) 
  add_subdirectory(src) 
  add_subdirectory(test) 
endif(QT4_FOUND AND KDE4_FOUND AND Redland_FOUND) 
