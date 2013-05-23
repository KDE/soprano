project(soprano_core)

# overriding QT_USE_FILE since that includes QT_INCLUDE_DIR before soprano headers
set(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
include_directories(BEFORE
  ${soprano_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${QT_QTCORE_INCLUDE_DIR}
  ${QT_INCLUDE_DIR}
  ${soprano_core_SOURCE_DIR}
  )

configure_file(soprano-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/soprano-config.h)
configure_file(version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

set(soprano_SRCS
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
  model.cpp
  model.h
  parser.cpp
  parser.h
  serializer.cpp
  serializer.h
  pluginmanager.cpp
  pluginstub.cpp
  plugin.cpp
  qhashqurlcompat.cpp
  backend.cpp
  backend.h
  global.cpp
  error.cpp
  error.h
  literalvalue.cpp
  literalvalue.h
  languagetag.cpp
  languagetag.h
  locator.cpp
  locator.h
  datetime.cpp
  version.cpp
  filtermodel.h
  filtermodel.cpp
  storagemodel.h
  storagemodel.cpp
  rdfschemamodel.cpp
  bindingset.cpp
  sopranotypes.cpp
  graph.cpp
  datastream.cpp
  vocabulary/rdf.cpp
  vocabulary/rdfs.cpp
  vocabulary/xsd.cpp
  vocabulary/nrl.cpp
  vocabulary/nao.cpp
  vocabulary/xesam.cpp
  vocabulary/owl.cpp
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
  inifile.cpp
  desktopfile.cpp
  sopranopluginfile.cpp
  sopranodirs.cpp
  n3nodeparser.cpp
  util/extreadwritelock.cpp
  util/mutexmodel.cpp
  util/mutexstatementiteratorbackend.cpp
  util/mutexnodeiteratorbackend.cpp
  util/mutexqueryresultiteratorbackend.cpp
  util/simplestatementiterator.cpp
  util/simplenodeiterator.cpp
  util/dummymodel.cpp
  util/signalcachemodel.cpp
  util/readonlymodel.cpp
  util/asyncmodel.cpp
  util/asyncresult.cpp
  util/asynccommand.cpp
  util/asynciteratorbackend.cpp
  util/asyncquery.cpp
  )


add_library(soprano ${LIBRARY_TYPE} ${soprano_SRCS})

target_link_libraries(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES
  VERSION 4.3.0
  SOVERSION 4
  DEFINE_SYMBOL MAKE_SOPRANO_LIB
  INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
  )

install(TARGETS soprano EXPORT SopranoExport
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  )

install(FILES
  sopranomacros.h
  parser.h
  queryresultiterator.h
  queryresultiteratorbackend.h
  node.h
  statement.h
  statementiterator.h
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
  locator.h
  literalvalue.h
  languagetag.h
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  rdfschemamodel.h
  bindingset.h
  serializer.h
  sopranotypes.h
  iterator.h
  iteratorbackend.h
  nrlmodel.h
  graph.h
  datastream.h
  vocabulary/rdf.h
  vocabulary/rdfs.h
  vocabulary/xsd.h
  vocabulary/nrl.h
  vocabulary/nao.h
  vocabulary/xesam.h
  vocabulary/owl.h
  inference/inferencemodel.h
  inference/inferencerule.h
  inference/inferenceruleset.h
  inference/nodepattern.h
  inference/statementpattern.h
  inference/sil.h
  inference/inferenceruleparser.h
  soprano.h
  vocabulary.h
  util/dummymodel.h
  util/mutexmodel.h
  util/simplestatementiterator.h
  util/simplenodeiterator.h
  util/signalcachemodel.h
  util/readonlymodel.h
  util/asyncmodel.h
  util/asyncresult.h
  util/asyncquery.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano)
