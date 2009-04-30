project(soprano) 

cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules) 

##################  Soprano version  ################################
set(CMAKE_SOPRANO_VERSION_MAJOR 2)
set(CMAKE_SOPRANO_VERSION_MINOR 2)
set(CMAKE_SOPRANO_VERSION_RELEASE 66)
set(CMAKE_SOPRANO_VERSION_STRING "${CMAKE_SOPRANO_VERSION_MAJOR}.${CMAKE_SOPRANO_VERSION_MINOR}.${CMAKE_SOPRANO_VERSION_RELEASE}")


##################  User options  ################################

option(SOPRANO_BUILD_TESTS "Build all unit tests")
option(SOPRANO_DISABLE_REDLAND_BACKEND "Disable the compilation of the redland backend")
option(SOPRANO_DISABLE_SESAME2_BACKEND "Disable the compilation of the sesame2 backend")
option(SOPRANO_DISABLE_VIRTUOSO_BACKEND "Disable the compilation of the Virtuoso backend")
option(SOPRANO_DISABLE_CLUCENE_INDEX "Disable the compilation of the clucene-based full-text index")
option(SOPRANO_DISABLE_RAPTOR_PARSER "Disable the compilation of the raptor parser and serializer plugins (provides most Soprano parsers and serializers)")
option(SOPRANO_DISABLE_DBUS "Disable the D-Bus integration")
option(SOPRANO_BUILD_API_DOCS "Build the Soprano API documentation")

if(SOPRANO_BUILD_TESTS)
  enable_testing()
endif(SOPRANO_BUILD_TESTS)

if(WIN32)
  set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

##################  find packages  ################################

set(QT_MIN_VERSION "4.4.0")
find_package(Qt4 REQUIRED) 
# properly set up compile flags (QT_DEBUG/QT_NO_DEBUG, ...)
include(${QT_USE_FILE})

if(NOT SOPRANO_DISABLE_RAPTOR_PARSER OR NOT SOPRANO_DISABLE_REDLAND_BACKEND)
  find_package(Rasqal)
  find_package(Raptor)
  find_package(Redland)

  if(WIN32)
    if(NOT REDLAND_VERSION)
      set(REDLAND_VERSION "1.0.3")
      message(STATUS "Windows currently only has Redland version 1.0.3")
    endif(NOT REDLAND_VERSION)
  endif(WIN32)
endif(NOT SOPRANO_DISABLE_RAPTOR_PARSER OR NOT SOPRANO_DISABLE_REDLAND_BACKEND)

if(NOT SOPRANO_DISABLE_CLUCENE_INDEX)
  find_package(CLucene)
  if(CLucene_FOUND)
    if(CLUCENE_VERSION AND CLUCENE_VERSION STRGREATER "0.9.19" OR CLUCENE_VERSION STREQUAL "0.9.19")
      set(CL_VERSION_19_OR_GREATER TRUE)
    endif(CLUCENE_VERSION AND CLUCENE_VERSION STRGREATER "0.9.19" OR CLUCENE_VERSION STREQUAL "0.9.19")
    set(SOPRANO_BUILD_INDEX_LIB 1 CACHE INTERNAL "Soprano Index is built" FORCE)
  endif(CLucene_FOUND)
endif(NOT SOPRANO_DISABLE_CLUCENE_INDEX)

if(NOT SOPRANO_DISABLE_SESAME2_BACKEND)
  find_package(JNI)
  if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    file(READ ${JAVA_INCLUDE_PATH}/jni.h jni_header_data)
    string(REGEX MATCH "JNI_VERSION_1_4" JNI_1_4_FOUND "${jni_header_data}")
    if(JNI_1_4_FOUND)
      message(STATUS "Found Java JNI >= 1.4: ${JAVA_INCLUDE_PATH}, ${JAVA_JVM_LIBRARY}")
    else(JNI_1_4_FOUND)
      message( "Need JNI version 1.4 or higher for the Sesame2 backend.")
    endif(JNI_1_4_FOUND)
  else(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    message(STATUS "Could not find Java JNI")
    if("$ENV{JAVA_HOME}" STREQUAL "")
      message("Make sure JAVA_HOME is set")
    endif("$ENV{JAVA_HOME}" STREQUAL "")
  endif(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
endif(NOT SOPRANO_DISABLE_SESAME2_BACKEND)

if(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND AND NOT WIN32)
  find_package(IODBC)
endif(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND AND NOT WIN32)


##################  setup variables  ################################
if(REDLAND_FOUND AND NOT SOPRANO_DISABLE_REDLAND_BACKEND)
  set(BUILD_REDLAND_BACKEND TRUE)
endif(REDLAND_FOUND AND NOT SOPRANO_DISABLE_REDLAND_BACKEND)
if(JNI_1_4_FOUND AND NOT SOPRANO_DISABLE_SESAME2_BACKEND)
  set(BUILD_SESAME2_BACKEND TRUE)
endif(JNI_1_4_FOUND AND NOT SOPRANO_DISABLE_SESAME2_BACKEND)
if(SOPRANO_BUILD_INDEX_LIB AND NOT SOPRANO_DISABLE_CLUCENE_INDEX)
  set(BUILD_CLUCENE_INDEX TRUE)
endif(SOPRANO_BUILD_INDEX_LIB AND NOT SOPRANO_DISABLE_CLUCENE_INDEX)
if(RAPTOR_FOUND AND NOT SOPRANO_DISABLE_RAPTOR_PARSER)
  set(BUILD_RAPTOR_PARSER TRUE)
endif(RAPTOR_FOUND AND NOT SOPRANO_DISABLE_RAPTOR_PARSER)
if(RAPTOR_FOUND AND REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5" AND NOT SOPRANO_DISABLE_RAPTOR_PARSER)
  set(BUILD_RAPTOR_SERIALIZER TRUE)
endif(RAPTOR_FOUND AND REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5" AND NOT SOPRANO_DISABLE_RAPTOR_PARSER)
if(NOT SOPRANO_DISABLE_DBUS AND QT_QTDBUS_FOUND)
  set(BUILD_DBUS_SUPPORT TRUE)
endif(NOT SOPRANO_DISABLE_DBUS AND QT_QTDBUS_FOUND)
if(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND AND IODBC_FOUND AND NOT WIN32)
  set(BUILD_VIRTUOSO_BACKEND TRUE)
endif(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND AND IODBC_FOUND AND NOT WIN32)
set(HAVE_DBUS BUILD_DBUS_SUPPORT)

##################  setup install directories  ################################
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name")
## the following are directories where stuff will be installed to
set(INCLUDE_INSTALL_DIR      "${CMAKE_INSTALL_PREFIX}/include/" CACHE PATH "The subdirectory to the header prefix")
set(PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")


# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library 
# install directory. Alex
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
set(CMAKE_INSTALL_RPATH                ${LIB_DESTINATION} )

if(APPLE)
   set(CMAKE_INSTALL_NAME_DIR ${LIB_DESTINATION})
endif(APPLE)


##################  some compiler settings ################################
if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32 )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32 )
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -Zc:wchar_t-)
endif(MSVC)
if(MINGW)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif(MINGW)


##################  add subdirectories ################################
if(BUILD_CLUCENE_INDEX)
   add_subdirectory(index)
endif(BUILD_CLUCENE_INDEX)
add_subdirectory(soprano) 
add_subdirectory(backends)
add_subdirectory(parsers)
add_subdirectory(serializers)
add_subdirectory(server)
add_subdirectory(tools)
add_subdirectory(rules)
add_subdirectory(includes)
if(SOPRANO_BUILD_TESTS)
  add_subdirectory(test) 
endif(SOPRANO_BUILD_TESTS)


##################  create pkgconfig file  ################################
if(NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/soprano.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano.pc @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/soprano.pc DESTINATION ${PKGCONFIG_INSTALL_PREFIX})
endif(NOT WIN32)


##################  apidox ################################
if(SOPRANO_BUILD_API_DOCS)
  find_package(Doxygen)

  if(DOXYGEN_EXECUTABLE)
    configure_file(${soprano_SOURCE_DIR}/Doxyfile.cmake ${soprano_BINARY_DIR}/Doxyfile)

    if(EXISTS ${QT_DOC_DIR}/html)
      set(QTDOCS "${QT_DOC_DIR}/html")
    else(EXISTS ${QT_DOC_DIR}/html)
      set(QTDOCS "http://doc.trolltech.com/4.3/")
    endif(EXISTS ${QT_DOC_DIR}/html)

    add_custom_target(
      apidox ALL
      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
      COMMAND docs/html/installdox -l qt4.tag@${QTDOCS} docs/html/*.html)
  endif(DOXYGEN_EXECUTABLE)
endif(SOPRANO_BUILD_API_DOCS)


##################  status messages ################################
message("---------------------------------------------------------------------------------------")
message("-- Soprano Components that will be built:")
if(NOT BUILD_REDLAND_BACKEND AND NOT BUILD_SESAME2_BACKEND)
  message("   WARNING: No Soprano backends will be compiled!")
  message("   WARNING: Soprano will not be very useful without storage backends!")
endif(NOT BUILD_REDLAND_BACKEND AND NOT BUILD_SESAME2_BACKEND)
if(BUILD_REDLAND_BACKEND)
  message("   * Redland storage backend")
endif(BUILD_REDLAND_BACKEND)
if(BUILD_SESAME2_BACKEND)
  message("   * Sesame2 storage backend (java-based)")
endif(BUILD_SESAME2_BACKEND)
if(BUILD_VIRTUOSO_BACKEND)
  message("   * Virtuoso storage backend (Run-time dependancy: Virtuoso)")
endif(BUILD_VIRTUOSO_BACKEND)

if(BUILD_RAPTOR_PARSER)
  message("   * Raptor RDF parser")
  if(RAPTOR_HAVE_TRIG)
    message("     (including TriG parser)")
  endif(RAPTOR_HAVE_TRIG)
endif(BUILD_RAPTOR_PARSER)

if(BUILD_RAPTOR_SERIALIZER)
  message("   * Raptor RDF serializer")
endif(BUILD_RAPTOR_SERIALIZER)

if(BUILD_CLUCENE_INDEX)
  message("   * The CLucene-based full-text search index library")
endif(BUILD_CLUCENE_INDEX)

if(BUILD_DBUS_SUPPORT)
  message("   * D-Bus server/client support")
endif(BUILD_DBUS_SUPPORT)


message("")
message("-- Soprano Components that will NOT be built:")
if(NOT BUILD_REDLAND_BACKEND)
  message("   * Redland storage backend")
endif(NOT BUILD_REDLAND_BACKEND)
if(NOT BUILD_SESAME2_BACKEND)
  message("   * Sesame2 storage backend (java-based)")
endif(NOT BUILD_SESAME2_BACKEND)
if(NOT BUILD_VIRTUOSO_BACKEND)
  message("   * Virtuoso storage backend")
endif(NOT BUILD_VIRTUOSO_BACKEND)

if(NOT BUILD_RAPTOR_PARSER)
  message("   * Raptor RDF parser")
endif(NOT BUILD_RAPTOR_PARSER)

if(NOT BUILD_RAPTOR_SERIALIZER)
  message("   * Raptor RDF serializer")
  if(NOT SOPRANO_DISABLE_RAPTOR_PARSER AND REDLAND_FOUND AND REDLAND_VERSION STRLESS "1.0.6")
    message("     (The Raptor serializer depends on Redland 1.0.6 or greater.")
  endif(NOT SOPRANO_DISABLE_RAPTOR_PARSER AND REDLAND_FOUND AND REDLAND_VERSION STRLESS "1.0.6")
endif(NOT BUILD_RAPTOR_SERIALIZER)

if(NOT BUILD_CLUCENE_INDEX)
  message("   * The CLucene-based full-text search index library")
endif(NOT BUILD_CLUCENE_INDEX)

if(NOT BUILD_DBUS_SUPPORT)
  message("   * D-Bus server/client support")
endif(NOT BUILD_DBUS_SUPPORT)

message("---------------------------------------------------------------------------------------")
