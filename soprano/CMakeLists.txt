project(soprano_core)

if (APPLE)
 set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
 set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (APPLE)

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

SET(soprano_SRCS
  queryresultiterator.cpp
  queryresultiteratorbackend.cpp
  node.cpp
  statement.cpp
  statementiterator.cpp
  nodeiterator.cpp
  model.cpp
  parser.cpp
  parser2.cpp
  serializer.cpp
  serializer2.cpp
  pluginmanager.cpp
  pluginstub.cpp
  plugin.cpp
  backend.cpp
  global.cpp
  error.cpp
  literalvalue.cpp
  locator.cpp
  datetime.cpp
  version.cpp
  filtermodel.cpp
  filtertransaction.cpp
  storagemodel.cpp
  rdfschemamodel.cpp
  bindingset.cpp
  sopranotypes.cpp
  transaction.cpp
  transactionfactory.cpp
  vocabulary/rdf.cpp
  vocabulary/rdfs.cpp
  vocabulary/xsd.cpp
  vocabulary/nrl.cpp
  vocabulary/nao.cpp
  vocabulary/xesam.cpp
  vocabulary/owl.cpp
  inference/nodepattern.cpp
  inference/statementpattern.cpp
  inference/inferencerule.cpp
  inference/inferenceruleset.cpp
  inference/sil.cpp
  inference/inferencemodel.cpp
  inference/inferenceruleparser.cpp
  nrlmodel.cpp
  inifile.cpp
  desktopfile.cpp
  sopranopluginfile.cpp
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
  util/simpletransactionmodel.cpp
  util/simpletransaction.cpp
  util/simpletransactionstatementiteratorbackend.cpp
  )

qt4_automoc(${soprano_SRCS})

ADD_LIBRARY(soprano SHARED ${soprano_SRCS})

TARGET_LINK_LIBRARIES(soprano ${QT_QTCORE_LIBRARY})

set_target_properties(soprano PROPERTIES
  VERSION 4.2.0
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
  parser2.h
  queryresultiterator.h
  queryresultiteratorbackend.h
  node.h
  statement.h
  statementiterator.h
  nodeiterator.h
  model.h
  filtermodel.h
  filtertransaction.h
  storagemodel.h
  plugin.h
  pluginmanager.h
  global.h
  soprano_export.h
  backend.h
  error.h
  locator.h
  literalvalue.h
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  rdfschemamodel.h
  bindingset.h
  serializer.h
  serializer2.h
  sopranotypes.h
  sopranomacros.h
  iterator.h
  iteratorbackend.h
  transaction.h
  transactionfactory.h
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
  util/simpletransactionmodel.h
  DESTINATION include/soprano)
