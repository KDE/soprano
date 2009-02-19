project(soprano_backends)

# we do another check for BUILD_REDLAND_BACKEND in the subdir since the raptor serializer
# depends on a static lib built here
if(REDLAND_FOUND)
  add_subdirectory(redland)
endif(REDLAND_FOUND)

if(BUILD_SESAME2_BACKEND)
  add_subdirectory(sesame2)
endif(BUILD_SESAME2_BACKEND)

if(BUILD_VIRTUOSO_BACKEND)
  add_subdirectory(virtuoso)
endif(BUILD_VIRTUOSO_BACKEND)
