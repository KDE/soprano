project(soprano)

cmake_minimum_required(VERSION 2.6.4)
# disable warning about shadowing CMakeParseArguments.cmake
if(POLICY CMP0017)
  cmake_policy(SET CMP0017 NEW)
endif(POLICY CMP0017)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(CTest)
include(CTestConfig.cmake)

##################  Soprano version  ################################
set(CMAKE_SOPRANO_VERSION_MAJOR 2)
set(CMAKE_SOPRANO_VERSION_MINOR 7)
set(CMAKE_SOPRANO_VERSION_RELEASE 55)
set(CMAKE_SOPRANO_VERSION_RELEASE 6)
set(CMAKE_SOPRANO_VERSION_STRING "${CMAKE_SOPRANO_VERSION_MAJOR}.${CMAKE_SOPRANO_VERSION_MINOR}.${CMAKE_SOPRANO_VERSION_RELEASE}")


##################  User options  ################################

option(SOPRANO_BUILD_TESTS "Build all unit tests")
option(SOPRANO_DISABLE_REDLAND_BACKEND "Disable compilation of Redland storage backend")
option(SOPRANO_DISABLE_SESAME2_BACKEND "Disable compilation of Sesame2 storage backend")
option(SOPRANO_DISABLE_VIRTUOSO_BACKEND "Disable compilation of Virtuoso storage backend")
option(SOPRANO_DISABLE_CLUCENE_INDEX "Disable compilation of Clucene-based full-text index")
option(SOPRANO_DISABLE_RAPTOR_PARSER "Disable compilation of Raptor parser plugin")
option(SOPRANO_DISABLE_RAPTOR_SERIALIZER "Disable compilation of Raptor RDF serializer plugin")
option(SOPRANO_DISABLE_DBUS "Disable D-Bus integration")
option(SOPRANO_BUILD_API_DOCS "Build Soprano API documentation")

if(SOPRANO_BUILD_TESTS)
  enable_testing()
endif(SOPRANO_BUILD_TESTS)

if(WIN32)
  set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

include(SopranoCPack.cmake)

##################  find packages  ################################

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 REQUIRED)
# properly set up compile flags (QT_DEBUG/QT_NO_DEBUG, ...)
include(${QT_USE_FILE})

if(NOT SOPRANO_DISABLE_RAPTOR_PARSER OR NOT SOPRANO_DISABLE_RAPTOR_SERIALIZER OR NOT SOPRANO_DISABLE_REDLAND_BACKEND)
  find_package(Rasqal)
  find_package(Raptor 2.0.0)
  find_package(Redland)

  if(WIN32)
    if(NOT REDLAND_VERSION)
      set(REDLAND_VERSION "1.0.3")
      message(STATUS "Windows currently only has Redland version 1.0.3")
    endif(NOT REDLAND_VERSION)
  endif(WIN32)
endif(NOT SOPRANO_DISABLE_RAPTOR_PARSER OR NOT SOPRANO_DISABLE_RAPTOR_SERIALIZER OR NOT SOPRANO_DISABLE_REDLAND_BACKEND)

if(NOT SOPRANO_DISABLE_CLUCENE_INDEX)
  find_package(CLucene)
  if(CLucene_FOUND)
    if(CLUCENE_VERSION AND CLUCENE_VERSION VERSION_GREATER "0.9.19" OR CLUCENE_VERSION VERSION_EQUAL "0.9.19")
      set(CL_VERSION_19_OR_GREATER TRUE)
    endif(CLUCENE_VERSION AND CLUCENE_VERSION VERSION_GREATER "0.9.19" OR CLUCENE_VERSION VERSION_EQUAL "0.9.19")
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

if(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND)
if(WIN32)
  include(CheckIncludeFiles)
  check_include_files("windows.h;sql.h" HAVE_SQL_H)
else(WIN32)
  find_package(IODBC)
endif(WIN32)
endif(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND)


##################  setup variables  ################################
if(REDLAND_FOUND)
   macro_ensure_version( "1.0.5" ${REDLAND_VERSION} REDLAND_VERSION_GOOD )
endif(REDLAND_FOUND)

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
if(RAPTOR_FOUND AND NOT SOPRANO_DISABLE_RAPTOR_SERIALIZER)
  set(BUILD_RAPTOR_SERIALIZER TRUE)
endif(RAPTOR_FOUND AND NOT SOPRANO_DISABLE_RAPTOR_SERIALIZER)
if(NOT SOPRANO_DISABLE_DBUS AND QT_QTDBUS_FOUND)
  set(BUILD_DBUS_SUPPORT TRUE)
endif(NOT SOPRANO_DISABLE_DBUS AND QT_QTDBUS_FOUND)
if(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND)
  if(IODBC_FOUND OR HAVE_SQL_H)
    set(BUILD_VIRTUOSO_BACKEND TRUE)
  endif(IODBC_FOUND OR HAVE_SQL_H)
endif(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND)
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
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fvisibility=hidden __SOPRANO_HAVE_GCC_VISIBILITY)
    check_cxx_compiler_flag(-fvisibility-inlines-hidden __SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
    if(__SOPRANO_HAVE_GCC_VISIBILITY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif(__SOPRANO_HAVE_GCC_VISIBILITY)
    if(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
    endif(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32 )
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -Zc:wchar_t-)
endif(MSVC)
if(MINGW)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif(MINGW)

if(WINCE)
  set(LIBRARY_TYPE STATIC)
  add_definitions(-DSOPRANO_STATIC_LIBS)
else(WINCE)
  set(LIBRARY_TYPE SHARED)
endif(WINCE)

# Soprano will handle large files in LockFile (Virtuoso DB files)
add_definitions(-D_FILE_OFFSET_BITS=64)


##################  add subdirectories ################################
if(BUILD_CLUCENE_INDEX)
   add_subdirectory(index)
endif(BUILD_CLUCENE_INDEX)
add_subdirectory(soprano)
add_subdirectory(backends)
add_subdirectory(parsers)
add_subdirectory(serializers)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tools)
add_subdirectory(rules)
add_subdirectory(includes)
if(SOPRANO_BUILD_TESTS)
  add_subdirectory(test)
endif(SOPRANO_BUILD_TESTS)
add_subdirectory(cmake)


##################  create pkgconfig files  ################################
if(NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/soprano.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano.pc @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sopranoclient.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/sopranoclient.pc @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sopranoserver.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/sopranoserver.pc @ONLY)

  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/soprano.pc
    ${CMAKE_CURRENT_BINARY_DIR}/sopranoclient.pc
    ${CMAKE_CURRENT_BINARY_DIR}/sopranoserver.pc
    DESTINATION
    ${PKGCONFIG_INSTALL_PREFIX})

  if(BUILD_CLUCENE_INDEX)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sopranoindex.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/sopranoindex.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sopranoindex.pc DESTINATION ${PKGCONFIG_INSTALL_PREFIX})
  endif(BUILD_CLUCENE_INDEX)
endif(NOT WIN32)


##################  apidox ################################
find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  configure_file(${soprano_SOURCE_DIR}/Doxyfile.cmake ${soprano_BINARY_DIR}/Doxyfile)
  set(QTDOCS_ONLINE "http://doc.qt.nokia.com/latest/")

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "${QT_DOC_DIR}/html")
  else(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS ${QTDOCS_ONLINE})
  endif(EXISTS ${QT_DOC_DIR}/html)

  if(SOPRANO_BUILD_API_DOCS)
    add_custom_target(
      apidox ALL
      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
      COMMAND docs/html/installdox -q -l qt4.tag@${QTDOCS} docs/html/*.html
      COMMENT "Building Soprano API docs...")
  else(SOPRANO_BUILD_API_DOCS)
    add_custom_target(
      apidox
      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
      COMMAND docs/html/installdox -q -l qt4.tag@${QTDOCS} docs/html/*.html
      COMMENT "Building Soprano API docs...")
  endif(SOPRANO_BUILD_API_DOCS)

  add_custom_target(
    onlineapidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND docs/html/installdox -q -l qt4.tag@${QTDOCS_ONLINE} docs/html/*.html
    COMMENT "Building Soprano API docs...")

  add_custom_target(
    stabledox
    COMMAND rsync -zrtq docs/html/ $ENV{USER},soprano@web.sf.net:/home/project-web/soprano/htdocs/apidox/stable
    DEPENDS onlineapidox
    COMMENT "Uploading stable API docs to Sourceforge...")

  add_custom_target(
    trunkdox
    COMMAND rsync -zrtq docs/html/ $ENV{USER},soprano@web.sf.net:/home/project-web/soprano/htdocs/apidox/trunk
    DEPENDS onlineapidox
    COMMENT "Uploading trunk API docs to Sourceforge...")
endif(DOXYGEN_EXECUTABLE)


##################  status messages ################################
message("---------------------------------------------------------------------------------------")
message("-- Soprano Components that will be built:")
if(NOT BUILD_REDLAND_BACKEND AND NOT BUILD_SESAME2_BACKEND AND NOT BUILD_VIRTUOSO_BACKEND)
  message("   WARNING: No Soprano backends will be compiled!")
  message("   WARNING: Soprano will not be very useful without storage backends!")
endif(NOT BUILD_REDLAND_BACKEND AND NOT BUILD_SESAME2_BACKEND AND NOT BUILD_VIRTUOSO_BACKEND)
if(BUILD_REDLAND_BACKEND)
  message("   * Redland storage backend")
endif(BUILD_REDLAND_BACKEND)
if(BUILD_SESAME2_BACKEND)
  message("   * Sesame2 storage backend (java-based)")
endif(BUILD_SESAME2_BACKEND)
if(BUILD_VIRTUOSO_BACKEND)
  message("   * Virtuoso storage backend (Run-time dependency: Virtuoso)")
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
  message("   * Redland storage backend (missing librdf/redland - http://librdf.org)")
endif(NOT BUILD_REDLAND_BACKEND)
if(NOT BUILD_SESAME2_BACKEND)
  message("   * Sesame2 storage backend (java-based)")
endif(NOT BUILD_SESAME2_BACKEND)
if(NOT BUILD_VIRTUOSO_BACKEND)
  if(WIN32)
    message("   * Virtuoso storage backend (missing ODBC headers - http://msdn.microsoft.com/en-us/windows/bb980924.aspx)")
  else(WIN32)
    message("   * Virtuoso storage backend (missing libiodbc - http://www.iodbc.org)")
  endif(WIN32)
endif(NOT BUILD_VIRTUOSO_BACKEND)

if(NOT BUILD_RAPTOR_PARSER)
  message("   * Raptor RDF parser (missing libraptor - http://librdf.org)")
endif(NOT BUILD_RAPTOR_PARSER)

if(NOT BUILD_RAPTOR_SERIALIZER)
  message("   * Raptor RDF serializer (missing libraptor - http://librdf.org)")
  if(NOT SOPRANO_DISABLE_RAPTOR_PARSER AND REDLAND_FOUND AND REDLAND_VERSION VERSION_LESS "1.0.6")
    message("     (The Raptor serializer depends on librdf/redland 1.0.6 or greater.")
  endif(NOT SOPRANO_DISABLE_RAPTOR_PARSER AND REDLAND_FOUND AND REDLAND_VERSION VERSION_LESS "1.0.6")
endif(NOT BUILD_RAPTOR_SERIALIZER)

if(NOT BUILD_CLUCENE_INDEX)
  message("   * The CLucene-based full-text search index library")
endif(NOT BUILD_CLUCENE_INDEX)

if(NOT BUILD_DBUS_SUPPORT)
  message("   * D-Bus server/client support")
endif(NOT BUILD_DBUS_SUPPORT)

message("---------------------------------------------------------------------------------------")
