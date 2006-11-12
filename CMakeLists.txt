PROJECT(SOPRANO) 
 
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules) 
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib ) 
 
find_package(Qt4 REQUIRED) 
find_package(Redland REQUIRED) 
find_package(Raptor REQUIRED) 
find_package(Rasqal REQUIRED) 
find_package(KNep REQUIRED) 
 
if(QT4_FOUND AND Redland_FOUND AND Raptor_FOUND AND Rasqal_FOUND) 
  add_subdirectory(src) 
endif(QT4_FOUND AND Redland_FOUND AND Raptor_FOUND AND Rasqal_FOUND) 
