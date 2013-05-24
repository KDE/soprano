project(soprano_includes)

install(FILES
  Backend
  BackendFeature
  BackendOption
  BackendSetting
  BackendSettings
  BindingSet
  FilterModel
  Global
  Graph
  Iterator
  IteratorBackend
  LanguageTag
  LiteralValue
  Model
  NRLModel
  Node
  NodeIterator
  Parser
  Plugin
  PluginManager
  QueryResultIterator
  QueryResultIteratorBackend
  RdfSchemaModel
  RdfSerialization
  Serializer
  Soprano
  Statement
  StatementIterator
  StorageModel
  Version
  DESTINATION ${INCLUDE_INSTALL_DIR}/Soprano
  COMPONENT Devel
)

add_subdirectory(Error)
add_subdirectory(Server)
add_subdirectory(Client)
add_subdirectory(Inference)
add_subdirectory(Query)
add_subdirectory(Vocabulary)
add_subdirectory(Util)
if(SOPRANO_BUILD_INDEX_LIB)
  add_subdirectory(Index)
endif()
