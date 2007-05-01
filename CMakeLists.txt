project(soprano) 

enable_testing()
 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules) 

find_package(GNUWIN32) 
find_package(Qt4 REQUIRED) 

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

include_directories(${QT_QTCORE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR} ${QT_INCLUDE_DIR} )

add_subdirectory(soprano) 
add_subdirectory(test) 
