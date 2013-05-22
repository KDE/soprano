project(soprano_parsers)

if(BUILD_RAPTOR_PARSER)
  add_subdirectory(raptor)
endif()

add_subdirectory(nquads)
