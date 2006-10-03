PROJECT(QRDF)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )
set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib )

macro_optional_find_package(Redland)
macro_log_feature(RedLand_FOUND "Redland" "RedLand Development Libraries" "fixme" FALSE "" "fixme")

macro_optional_find_package(Raptor)
macro_log_feature(Raptor_FOUND "Raptor" "Raptor Development Libraries" "fixme" FALSE "" "fixme")

macro_optional_find_package(Rasqal)
macro_log_feature(Rasqal_FOUND "Rasqal" "Rasqal Development Libraries" "fixme" FALSE "" "fixme")

if(RedLand_FOUND AND Raptor_FOUND AND Rasqal_FOUND)
  add_subdirectory(src)
  add_subdirectory(devel)
  add_subdirectory(tests)
endif(RedLand_FOUND AND Raptor_FOUND AND Rasqal_FOUND)
