project(soprano)

cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# Used to report the packages we found
include(FeatureSummary)

# Enable CMake's Automoc
set(CMAKE_AUTOMOC ON)

# Automatically add the current source and build directories
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Used to set some generic paths
include(GNUInstallDirs)

# Used to create CMake config files
include(CMakePackageConfigHelpers)

include(CTest)
include(CTestConfig.cmake)

##################  Soprano version  ################################
set(CMAKE_SOPRANO_VERSION_MAJOR 2)
set(CMAKE_SOPRANO_VERSION_MINOR 9)
set(CMAKE_SOPRANO_VERSION_RELEASE 4)
set(CMAKE_SOPRANO_VERSION_STRING "${CMAKE_SOPRANO_VERSION_MAJOR}.${CMAKE_SOPRANO_VERSION_MINOR}.${CMAKE_SOPRANO_VERSION_RELEASE}")

# Set the SOVERSION
set(SOPRANO_GENERIC_SOVERSION "1")
set(SOPRANO_NON_GENERIC_SOVERSION "4")

##################  User options  ################################

option(QT5_BUILD "Build Soprano using the Qt5 framework" FALSE)
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
endif()

if(WIN32)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

include(SopranoCPack.cmake)

# properly set up compile flags (QT_DEBUG/QT_NO_DEBUG, ...)
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
endif()

# Place the built executables/libraries in the same directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
if(WIN32)
  set(LIBRARY_OUTPUT_PATH   ${EXECUTABLE_OUTPUT_PATH})
else()
  set(LIBRARY_OUTPUT_PATH   ${CMAKE_BINARY_DIR}/lib)
endif()

##################  find packages  ################################

if(QT5_BUILD)
  find_package(Qt5 COMPONENTS Core DBus Network Xml REQUIRED CONFIG)
  if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
  include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})

  # bump the SOVERSION when built vs Qt5.
  math(EXPR _SOPRANO_GENERIC_SOVERSION ${SOPRANO_GENERIC_SOVERSION}+1)
  math(EXPR _SOPRANO_NON_GENERIC_SOVERSION ${SOPRANO_NON_GENERIC_SOVERSION}+1)
  set(SOPRANO_GENERIC_SOVERSION "${_SOPRANO_GENERIC_SOVERSION}")
  set(SOPRANO_NON_GENERIC_SOVERSION "${_SOPRANO_NON_GENERIC_SOVERSION}")
else()
  set(QT_USE_IMPORTED_TARGETS TRUE)
  find_package(Qt4 4.5.0)
  set_package_properties(Qt4 PROPERTIES
                        DESCRIPTION "The Qt4 libraries"
                        URL "http://qt-project.org/"
                        TYPE REQUIRED
  )

  if (NOT CMAKE_BUILD_TYPE_TOLOWER MATCHES "debug")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
  endif()

  include(${QT_USE_FILE})
endif()

if(NOT SOPRANO_DISABLE_RAPTOR_PARSER OR NOT SOPRANO_DISABLE_RAPTOR_SERIALIZER OR NOT SOPRANO_DISABLE_REDLAND_BACKEND)
  find_package(Rasqal)
  set_package_properties(Rasqal PROPERTIES
                         URL "http://librdf.org/rasqal"
                         DESCRIPTION "Rasqal RDF Query Library"
                         TYPE RECOMMENDED
                         PURPOSE "Needed to build the Raptor parser and Redland backend"
  )

  find_package(Raptor 2.0.0)
  set_package_properties(Raptor PROPERTIES
                         URL "http://librdf.org/raptor"
                         DESCRIPTION "Raptor RDF Syntax Library"
                         TYPE RECOMMENDED
                         PURPOSE "Needed to build the Raptor parser and Redland backend"
  )

  find_package(Redland 1.0.5)
  set_package_properties(Redland PROPERTIES
                         URL "http://librdf.org"
                         DESCRIPTION "Support for the Resource Description Framework (RDF)"
                         TYPE RECOMMENDED
                         PURPOSE "Needed to build the Raptor parser and Redland backend"
  )

  if(WIN32)
    if(NOT REDLAND_VERSION)
      set(REDLAND_VERSION "1.0.3")
      message(STATUS "Windows currently only has Redland version 1.0.3")
    endif()
  endif()
endif()

# disable indexer if not explicitly enabled below
set(SOPRANO_BUILD_INDEX_LIB 0 CACHE INTERNAL "Soprano Index is not built" FORCE)
if(NOT SOPRANO_DISABLE_CLUCENE_INDEX)
  find_package(CLucene)
  set_package_properties(CLucene PROPERTIES
                         URL "http://clucene.sourceforge.net/"
                         DESCRIPTION "High-performance indexing and searching API"
                         TYPE OPTIONAL
  )

  if(CLucene_FOUND)
    if(CLUCENE_VERSION AND (NOT CLUCENE_VERSION VERSION_LESS 0.9.19))
      set(CL_VERSION_19_OR_GREATER TRUE)
    endif()
    set(SOPRANO_BUILD_INDEX_LIB 1 CACHE INTERNAL "Soprano Index is built" FORCE)
  endif()
endif()

if(NOT SOPRANO_DISABLE_SESAME2_BACKEND)
  find_package(JNI)
  set_package_properties(JNI PROPERTIES
                         URL "https://www.java.com"
                         DESCRIPTION "Java libraries"
  )

  if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    file(READ ${JAVA_INCLUDE_PATH}/jni.h jni_header_data)
    string(REGEX MATCH "JNI_VERSION_1_4" JNI_1_4_FOUND "${jni_header_data}")
    if(JNI_1_4_FOUND)
      message(STATUS "Found Java JNI >= 1.4: ${JAVA_INCLUDE_PATH}, ${JAVA_JVM_LIBRARY}")
    else()
      message( "Need JNI version 1.4 or higher for the Sesame2 backend.")
    endif()
  else()
    message(STATUS "Could not find Java JNI")
    if("$ENV{JAVA_HOME}" STREQUAL "")
      message("Make sure JAVA_HOME is set")
    endif()
  endif()
endif()

if(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND)
  if(WIN32)
    include(CheckIncludeFiles)
    check_include_files("windows.h;sql.h" HAVE_SQL_H)
  else()
    find_package(IODBC)
    set_package_properties(IODBC PROPERTIES
                          URL "http://www.iodbc.org"
                          DESCRIPTION "The iODBC Driver Manager"
                          TYPE RECOMMENDED
                          PURPOSE "Needed to build the Virtuoso backend"
    )
  endif()
endif()

##################  setup variables  ################################

if(REDLAND_FOUND AND NOT SOPRANO_DISABLE_REDLAND_BACKEND)
  set(BUILD_REDLAND_BACKEND TRUE)
endif()
if(JNI_1_4_FOUND AND NOT SOPRANO_DISABLE_SESAME2_BACKEND)
  set(BUILD_SESAME2_BACKEND TRUE)
endif()
if(SOPRANO_BUILD_INDEX_LIB AND NOT SOPRANO_DISABLE_CLUCENE_INDEX)
  set(BUILD_CLUCENE_INDEX TRUE)
endif()
if(RAPTOR_FOUND AND NOT SOPRANO_DISABLE_RAPTOR_PARSER)
  set(BUILD_RAPTOR_PARSER TRUE)
endif()
if(RAPTOR_FOUND AND NOT SOPRANO_DISABLE_RAPTOR_SERIALIZER)
  set(BUILD_RAPTOR_SERIALIZER TRUE)
endif()
if(NOT SOPRANO_DISABLE_DBUS AND (QT_QTDBUS_FOUND OR Qt5DBus_FOUND))
  set(BUILD_DBUS_SUPPORT TRUE)
endif()
if(NOT SOPRANO_DISABLE_VIRTUOSO_BACKEND)
  if(IODBC_FOUND OR HAVE_SQL_H)
    set(BUILD_VIRTUOSO_BACKEND TRUE)
  endif()
endif()
set(HAVE_DBUS BUILD_DBUS_SUPPORT)

##################  setup install directories  ################################

# the following are directories where stuff will be installed to
set(BIN_INSTALL_DIR          "${CMAKE_INSTALL_FULL_BINDIR}" CACHE PATH "The binaries directory")
set(DATA_INSTALL_DIR         "${CMAKE_INSTALL_FULL_DATADIR}" CACHE PATH "The data directory")
set(INCLUDE_INSTALL_DIR      "${CMAKE_INSTALL_FULL_INCLUDEDIR}" CACHE PATH "The subdirectory to the header prefix")
set(LIB_INSTALL_DIR         "${CMAKE_INSTALL_FULL_LIBDIR}" CACHE STRING "Library directory name")
set(PKGCONFIG_INSTALL_PREFIX "${LIB_INSTALL_DIR}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")

# Where to install libraries and binaries
set(INSTALL_TARGETS_DEFAULT_ARGS RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                 LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                 ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel
   )

# Where to install plugins
if(WIN32)
  # On Windows, plugins are installed somewhere in the binary dir
  set(PLUGIN_INSTALL_DIR DESTINATION ${BIN_INSTALL_DIR}/soprano)
else()
  set(PLUGIN_INSTALL_DIR RUNTIME DESTINATION "${BIN_INSTALL_DIR}/soprano"
                         LIBRARY DESTINATION "${LIB_INSTALL_DIR}/soprano"
                         ARCHIVE DESTINATION "${LIB_INSTALL_DIR}/soprano"
     )
endif()

# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library
# install directory. Alex
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemCLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
if("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")
set(CMAKE_INSTALL_RPATH                ${LIB_INSTALL_DIR} )
endif()

if(APPLE)
   set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
endif()


##################  some compiler settings ################################
if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32 )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fvisibility=hidden __SOPRANO_HAVE_GCC_VISIBILITY)
    check_cxx_compiler_flag(-fvisibility-inlines-hidden __SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
    if(__SOPRANO_HAVE_GCC_VISIBILITY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
    if(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
    endif()
endif()
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -Zc:wchar_t-)
endif()
if(MINGW)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif()

if(WINCE)
  set(LIBRARY_TYPE STATIC)
  add_definitions(-DSOPRANO_STATIC_LIBS)
else()
  set(LIBRARY_TYPE SHARED)
endif()

# Soprano will handle large files in LockFile (Virtuoso DB files)
add_definitions(-D_FILE_OFFSET_BITS=64)


##################  add subdirectories ################################
if(BUILD_CLUCENE_INDEX)
   add_subdirectory(index)
endif()
add_subdirectory(soprano)
add_subdirectory(backends)
add_subdirectory(parsers)
add_subdirectory(serializers)
add_subdirectory(server)
if(NOT QT5_BUILD)
add_subdirectory(client)
else()
message(STATUS "libsopranoclient needs some porting")
endif()
add_subdirectory(tools)
add_subdirectory(rules)
add_subdirectory(includes)
if(SOPRANO_BUILD_TESTS)
  add_subdirectory(test)
endif()
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
  endif()
endif()


##################  apidox ##################
find_package(Doxygen)
set_package_properties(Doxygen PROPERTIES
                       URL "http://www.stack.nl/~dimitri/doxygen/"
                       DESCRIPTION "Automated C, C++, and Java Documentation Generator"
)

if(DOXYGEN_EXECUTABLE)
  set(QTDOCS_ONLINE "http://qt-project.org/doc/qt-4.8/")

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS_LOCAL "${QT_DOC_DIR}/html")
  else()
    set(QTDOCS_LOCAL ${QTDOCS_ONLINE})
  endif()

  set(QTDOCS ${QTDOCS_LOCAL})
  configure_file(${soprano_SOURCE_DIR}/Doxyfile.cmake ${soprano_BINARY_DIR}/Doxyfile.local)

  set(QTDOCS ${QTDOCS_ONLINE})
  configure_file(${soprano_SOURCE_DIR}/Doxyfile.cmake ${soprano_BINARY_DIR}/Doxyfile.online)

  if(SOPRANO_BUILD_API_DOCS)
    add_custom_target(
      apidox ALL
      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile.local
      COMMENT "Building Soprano API docs...")
  else()
    add_custom_target(
      apidox
      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile.local
      COMMENT "Building Soprano API docs...")
  endif()

  add_custom_target(
    onlineapidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile.online
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
endif()


##################  status messages ################################
if(BUILD_REDLAND_BACKEND)
  set(Soprano_PLUGIN_REDLANDBACKEND_FOUND TRUE)
else()
  set(Soprano_PLUGIN_REDLANDBACKEND_FOUND FALSE)
endif()
if(BUILD_SESAME2_BACKEND)
  set(Soprano_PLUGIN_SESAME2BACKEND_FOUND TRUE)
else()
  set(Soprano_PLUGIN_SESAME2BACKEND_FOUND FALSE)
endif()
if(BUILD_VIRTUOSO_BACKEND)
  set(Soprano_PLUGIN_VIRTUOSOBACKEND_FOUND TRUE)
else()
  set(Soprano_PLUGIN_VIRTUOSOBACKEND_FOUND FALSE)
endif()
if(BUILD_RAPTOR_PARSER)
  set(Soprano_PLUGIN_RAPTORPARSER_FOUND TRUE)
else()
    set(Soprano_PLUGIN_RAPTORPARSER_FOUND FALSE)
endif()
if(BUILD_RAPTOR_SERIALIZER)
  set(Soprano_PLUGIN_RAPTORSERIALIZER_FOUND TRUE)
else()
  set(Soprano_PLUGIN_RAPTORSERIALIZER_FOUND FALSE)
endif()

add_feature_info("Redland storage backend" BUILD_REDLAND_BACKEND
                 "STRONGLY RECOMMENDED: The redland backend is required to build Nepomuk."
                )

if(WIN32)
  set(_virtuoso_extra_feature_info "Requires the ODBC headers - http://msdn.microsoft.com/en-us/windows/bb980924.aspx")
endif()
add_feature_info("Virtuoso storage backend" BUILD_VIRTUOSO_BACKEND
                 "STRONGLY RECOMMENDED: The Virtuoso backend is required to build Nepomuk.
                 ${_virtuoso_extra_feature_info}"
                )

add_feature_info("Raptor RDF parser" BUILD_RAPTOR_PARSER
                 "STRONGLY RECOMMENDED: The Raptor RDF parser is required to build Nepomuk."
                )

add_feature_info("Raptor RDF serializer" BUILD_RAPTOR_SERIALIZER
                 "Raptor RDF serializer"
                )

add_feature_info("CLucene library" BUILD_CLUCENE_INDEX
                 "The CLucene-based full-text search index library"
                )

add_feature_info("Sesame2 storage backend (java-based)" BUILD_SESAME2_BACKEND
                 "Optional: The Virtuoso backend should be used instead.")

add_feature_info("D-Bus server/client support" BUILD_DBUS_SUPPORT "")


feature_summary(WHAT ALL
                     INCLUDE_QUIET_PACKAGES
                     FATAL_ON_MISSING_REQUIRED_PACKAGES
               )

##################  Write SopranoConfigVersion.cmake ##################
write_basic_package_version_file(${soprano_BINARY_DIR}/SopranoConfigVersion.cmake
                                 VERSION ${SOPRANO_VERSION_STRING}
                                 COMPATIBILITY SameMajorVersion
                                )

##################  Write SopranoConfig.cmake ##################
# Create the CMake Config files
configure_package_config_file(SopranoConfig.cmake.in
                              ${soprano_BINARY_DIR}/SopranoConfig.cmake
                              INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/Soprano
                              PATH_VARS BIN_INSTALL_DIR DATA_INSTALL_DIR INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)

install(FILES ${soprano_BINARY_DIR}/SopranoConfig.cmake
              ${soprano_BINARY_DIR}/SopranoConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/cmake/Soprano
       )

install(EXPORT SopranoExport FILE SopranoTargets.cmake
        DESTINATION ${LIB_INSTALL_DIR}/cmake/Soprano
        NAMESPACE Soprano::)
