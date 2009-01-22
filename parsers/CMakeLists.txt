project(soprano_parsers)

if(REDLAND_FOUND)
  add_subdirectory(raptor)
endif(REDLAND_FOUND)

add_subdirectory(nquads)
