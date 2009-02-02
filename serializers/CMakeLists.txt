project(soprano_parsers)

if(BUILD_RAPTOR_SERIALIZER)
  add_subdirectory(raptor)
endif(BUILD_RAPTOR_SERIALIZER)

add_subdirectory(nquads)
