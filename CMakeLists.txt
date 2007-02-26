project(Soprano) 

enable_testing()
 
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules) 

find_package(KDE4 REQUIRED) 
find_package(Redland REQUIRED) 
	 
include_directories(${QT_QTCORE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR} ${QT_INCLUDE_DIR} )

if(QT4_FOUND AND KDE4_FOUND AND Redland_FOUND) 
  add_subdirectory(soprano) 
  add_subdirectory(test) 
endif(QT4_FOUND AND KDE4_FOUND AND Redland_FOUND) 
