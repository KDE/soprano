project(soprano_parsers)

if(BUILD_RAPTOR_PARSER)
  add_subdirectory(raptor)
endif(BUILD_RAPTOR_PARSER)

add_subdirectory(nquads)
