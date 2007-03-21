project(Soprano) 

enable_testing()
 
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules) 

find_package(Qt4 REQUIRED) 
	 
include_directories(${QT_QTCORE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR} ${QT_INCLUDE_DIR} )

add_subdirectory(soprano) 
add_subdirectory(test) 
