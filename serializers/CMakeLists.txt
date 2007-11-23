project(soprano_parsers)

if(WIN32)
  set(REDLAND_VERSION "1.0.3")
  message(STATUS "Windows currently oly has Redland version 1.0.3")
endif(WIN32)

if(REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5")
  add_subdirectory(raptor)
else(REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5")
  if(REDLAND_FOUND)
    message(STATUS "Redland version less than 1.0.6: ${redland_VERSION}")
    message(STATUS "The redland serializer plugin will not be built!")
  endif(REDLAND_FOUND)
endif(REDLAND_FOUND AND REDLAND_VERSION STRGREATER "1.0.5")

add_subdirectory(nquads)
