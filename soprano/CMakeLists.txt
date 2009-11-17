project(soprano_core)

include_directories(
  ${soprano_SOURCE_DIR}
  ${soprano_core_BINARY_DIR}
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${QT_QTCORE_INCLUDE_DIR}
  ${QT_INCLUDE_DIR}
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

qt4_automoc(${soprano_SRCS})

add_library(soprano SHARED ${soprano_SRCS})

target_link_libraries(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES
  VERSION 4.2.0
  SOVERSION 4
  DEFINE_SYMBOL MAKE_SOPRANO_LIB
  INSTALL_NAME_DIR ${LIB_DESTINATION}
  )

install(TARGETS soprano
  LIBRARY DESTINATION ${LIB_DESTINATION}
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
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
#  query/query.h
#  query/queryparser.h
#  query/queryserializer.h
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
  DESTINATION include/soprano)
