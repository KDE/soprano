PROJECT(QRDF) 
 
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules) 
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib ) 
 
find_package(Qt4 REQUIRED) 
 
find_package(Redland REQUIRED) 
 
find_package(Raptor REQUIRED) 
 
find_package(Rasqal REQUIRED) 
 
if(QT4_FOUND AND Redland_FOUND AND Raptor_FOUND AND Rasqal_FOUND) 
  add_subdirectory(src) 
#  add_subdirectory(samples) 
#  add_subdirectory(tests) 
endif(QT4_FOUND AND Redland_FOUND AND Raptor_FOUND AND Rasqal_FOUND) 
