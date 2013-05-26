project(soprano_core)

# overriding QT_USE_FILE since that includes QT_INCLUDE_DIR before soprano headers
set(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
include_directories(BEFORE
  ${soprano_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
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

if(QT5_BUILD)
  target_link_libraries(soprano ${Qt5Core_LIBRARIES})
else()
  target_link_libraries(soprano ${QT_QTCORE_LIBRARY})
endif()

set_target_properties(soprano PROPERTIES
  VERSION 4.3.0
  SOVERSION 4
  DEFINE_SYMBOL MAKE_SOPRANO_LIB
  INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
  )

install(TARGETS soprano EXPORT SopranoExport ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  backend.h
  bindingset.h
  datastream.h
  error.h
  filtermodel.h
  global.h
  graph.h
  inference/inferencemodel.h
  inference/inferencerule.h
  inference/inferenceruleparser.h
  inference/inferenceruleset.h
  inference/nodepattern.h
  inference/sil.h
  inference/statementpattern.h
  iterator.h
  iteratorbackend.h
  languagetag.h
  literalvalue.h
  locator.h
  model.h
  node.h
  nodeiterator.h
  nrlmodel.h
  parser.h
  plugin.h
  pluginmanager.h
  queryresultiterator.h
  queryresultiteratorbackend.h
  rdfschemamodel.h
  serializer.h
  soprano.h
  soprano_export.h
  sopranomacros.h
  sopranotypes.h
  statement.h
  statementiterator.h
  storagemodel.h
  util/asyncmodel.h
  util/asyncquery.h
  util/asyncresult.h
  util/dummymodel.h
  util/mutexmodel.h
  util/readonlymodel.h
  util/signalcachemodel.h
  util/simplenodeiterator.h
  util/simplestatementiterator.h
  vocabulary.h
  vocabulary/nao.h
  vocabulary/nrl.h
  vocabulary/owl.h
  vocabulary/rdf.h
  vocabulary/rdfs.h
  vocabulary/xesam.h
  vocabulary/xsd.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
  COMPONENT Devel
)
