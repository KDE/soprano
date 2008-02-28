project(soprano) 

set(CMAKE_SOPRANO_VERSION_MAJOR 2 CACHE INT "Major Soprano version number" FORCE)
set(CMAKE_SOPRANO_VERSION_MINOR 0 CACHE INT "Minor Soprano version number" FORCE)
set(CMAKE_SOPRANO_VERSION_RELEASE 3 CACHE INT "Release Soprano version number" FORCE)
set(CMAKE_SOPRANO_VERSION_STRING "${CMAKE_SOPRANO_VERSION_MAJOR}.${CMAKE_SOPRANO_VERSION_MINOR}.${CMAKE_SOPRANO_VERSION_RELEASE}" CACHE STRING "Soprano version string" FORCE)

enable_testing()
 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules) 

if(WIN32)
  set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

find_package(Qt4 REQUIRED) 

find_package(Redland)

find_package(JNI)
if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
  file(READ ${JAVA_INCLUDE_PATH}/jni.h jni_header_data)
  string(REGEX MATCH "JNI_VERSION_1_6" JNI_1_6_FOUND "${jni_header_data}")
  if(JNI_1_6_FOUND)
    message(STATUS "Found Java JNI >= 1.6: ${JAVA_INCLUDE_PATH}, ${JAVA_JVM_LIBRARY}")
  else(JNI_1_6_FOUND)
    message( "Need JRE version 1.6 or higher for the Sesame2 backend.")
  endif(JNI_1_6_FOUND)
else(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
  message(STATUS "Could not find Java JNI")
endif(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)
## the following are directories where stuff will be installed to
set(INCLUDE_INSTALL_DIR      "${CMAKE_INSTALL_PREFIX}/include/"
CACHE PATH "The subdirectory to the header prefix" FORCE)



if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32 )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32 )
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -Zc:wchar_t-)
endif(MSVC)
if(MINGW)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif(MINGW)

find_package(CLucene)
if(CLucene_FOUND)
  IF(CLUCENE_VERSION AND CLUCENE_VERSION STRGREATER "0.9.19" OR CLUCENE_VERSION STREQUAL "0.9.19")
    SET(CL_VERSION_19_OR_GREATER TRUE)
  ENDIF(CLUCENE_VERSION AND CLUCENE_VERSION STRGREATER "0.9.19" OR CLUCENE_VERSION STREQUAL "0.9.19")
  set(SOPRANO_BUILD_INDEX_LIB 1 CACHE INTERNAL "Soprano Index is built" FORCE)
  add_subdirectory(index)
endif(CLucene_FOUND)

#PkgConfig
if(NOT WIN32)
  set(PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/soprano.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano.pc @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/soprano.pc DESTINATION ${PKGCONFIG_INSTALL_PREFIX})
endif(NOT WIN32)

add_subdirectory(soprano) 
add_subdirectory(backends)
add_subdirectory(parsers)
add_subdirectory(serializers)
#add_subdirectory(queryparsers)
add_subdirectory(server)
add_subdirectory(tools)
add_subdirectory(test) 
add_subdirectory(rules)
add_subdirectory(includes)

# apidox
find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  configure_file(${soprano_SOURCE_DIR}/Doxyfile.cmake ${soprano_BINARY_DIR}/Doxyfile)

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "${QT_DOC_DIR}/html")
  else(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "http://doc.trolltech.com/4.3/")
  endif(EXISTS ${QT_DOC_DIR}/html)

  add_custom_target(
    apidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND docs/html/installdox -l qt4.tag@${QTDOCS} docs/html/*.html)
endif(DOXYGEN_EXECUTABLE)

message("---------------------------------------------------------------------------------------")
message("-- Soprano Components that will be built:")
if(NOT REDLAND_FOUND AND JNI_1_6_FOUND)
  message("   WARNING: No Soprano backends will be compiled due to missing dependancies!")
  message("   WARNING: Soprano will not be very useful without storage backends!")
endif(NOT REDLAND_FOUND AND JNI_1_6_FOUND)
if(REDLAND_FOUND)
  message("   * Redland storage backend")
endif(REDLAND_FOUND)
if(JNI_1_6_FOUND)
  message("   * Sesame2 storage backend (java-based)")
endif(JNI_1_6_FOUND)

if(REDLAND_FOUND)
  message("   * Raptor RDF parser")
  if(RAPTOR_HAVE_TRIG)
    message("     (including TriG parser)")
  endif(RAPTOR_HAVE_TRIG)
endif(REDLAND_FOUND)

if(REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5")
  message("   * Raptor RDF serializer")
endif(REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5")

if(CLucene_FOUND)
  message("   * The CLucene-based full-text search index library")
endif(CLucene_FOUND)


message("")
message("-- Soprano Components that will NOT be built:")
if(NOT REDLAND_FOUND)
  message("   * Redland storage backend")
endif(NOT REDLAND_FOUND)
if(NOT JNI_1_6_FOUND)
  message("   * Sesame2 storage backend (java-based)")
endif(NOT JNI_1_6_FOUND)

if(NOT REDLAND_FOUND)
  message("   * Raptor RDF parser")
endif(NOT REDLAND_FOUND)

if(NOT REDLAND_FOUND OR REDLAND_VERSION STRLESS "1.0.6")
  message("   * Raptor RDF serializer")
endif(NOT REDLAND_FOUND OR REDLAND_VERSION STRLESS "1.0.6")

if(NOT CLucene_FOUND)
  message("   * The CLucene-based full-text search index library")
endif(NOT CLucene_FOUND)

message("---------------------------------------------------------------------------------------")
