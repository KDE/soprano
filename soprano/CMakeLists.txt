project(soprano_core)

if (WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif (WIN32)
 
if (APPLE)
 set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
 set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (APPLE)

include_directories(
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${CMAKE_SOURCE_DIR}/soprano
  ${QT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  )

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

SET(soprano_SRCS
  queryresultiterator.cpp
  queryresultiterator.h
  queryresultiteratorbackend.cpp
  queryresultiteratorbackend.h
  node.cpp
  node.h
  statement.cpp
  statement.h
  statementiterator.cpp
  statementiterator.h
  nodeiterator.cpp
  simplestatementiterator.cpp
  model.cpp
  model.h
  parser.cpp
  parser.h
  serializer.cpp
  serializer.h
  pluginmanager.cpp
  pluginmanager.h
  plugin.cpp
  backend.cpp
  backend.h
  global.cpp
  error.cpp
  error.h
  literalvalue.cpp
  literalvalue.h
  locator.cpp
  locator.h
  datetime.cpp
  version.cpp
  filtermodel.h
  filtermodel.cpp
  storagemodel.h
  storagemodel.cpp
  rdfschemawrapper.cpp
  simplestatementiterator.h
  bindingset.cpp
  sopranotypes.cpp
  vocabulary/rdf.cpp
  vocabulary/rdfs.cpp
  vocabulary/xmls.cpp
  vocabulary/nrl.cpp
  query/query.cpp
  query/queryparser.cpp
  query/queryserializer.cpp
  inference/nodepattern.cpp
  inference/statementpattern.cpp
  inference/inferencerule.cpp
  inference/inferenceruleset.cpp
  inference/sil.cpp
  inference/inferencemodel.h
  inference/inferencemodel.cpp
  inference/inferenceruleparser.cpp
  nrlmodel.cpp
  )

qt4_automoc(${soprano_SRCS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY} rasqal raptor)

set_target_properties(soprano PROPERTIES
  VERSION 4.0.0
  SOVERSION 4
  DEFINE_SYMBOL MAKE_SOPRANO_LIB
  INSTALL_NAME_DIR ${LIB_DESTINATION}
  )

INSTALL(TARGETS soprano
  LIBRARY DESTINATION ${LIB_DESTINATION}
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  )

INSTALL(FILES 
  parser.h
  queryresultiterator.h
  queryresultiteratorbackend.h
  node.h
  statement.h
  statementiterator.h
  simplestatementiterator.h
  nodeiterator.h
  model.h
  filtermodel.h
  storagemodel.h
  plugin.h
  pluginmanager.h
  global.h
  soprano_export.h
  backend.h
  error.h
  literalvalue.h
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  rdfschemawrapper.h
  bindingset.h
  serializer.h
  sopranotypes.h
  iterator.h
  iteratorbackend.h
  vocabulary/rdf.h
  vocabulary/rdfs.h
  vocabulary/xmls.h
  vocabulary/nrl.h
  inference/inferencemodel.h
  inference/inferencerule.h
  inference/inferenceruleset.h
  inference/nodepattern.h
  inference/statementpattern.h
  inference/sil.h
  inference/inferenceruleparser.h
  query/query.h
  query/queryparser.h
  query/queryserializer.h
  soprano.h
  vocabulary.h
  DESTINATION include/soprano)
