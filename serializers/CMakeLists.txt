project(soprano_parsers)

if(BUILD_RAPTOR_SERIALIZER)
  add_subdirectory(raptor)
endif()

add_subdirectory(nquads)
