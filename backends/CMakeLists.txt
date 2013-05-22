project(soprano_backends)

# we do another check for BUILD_REDLAND_BACKEND in the subdir since the raptor serializer
# depends on a static lib built here
if(REDLAND_FOUND)
  add_subdirectory(redland)
endif()

if(BUILD_SESAME2_BACKEND)
  add_subdirectory(sesame2)
endif()

if(BUILD_VIRTUOSO_BACKEND)
  add_subdirectory(virtuoso)
endif()
