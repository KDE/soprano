PROJECT(QRDF)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

macro_optional_find_package(Redland)
macro_log_feature(RedLand_FOUND "Redland" "RedLand Development Libraries" "fixme" FALSE "" "fixme")

macro_optional_find_package(Raptor)
macro_log_feature(Raptor_FOUND "Raptor" "Raptor Development Libraries" "fixme" FALSE "" "fixme")

macro_optional_find_package(Rasqal)
macro_log_feature(Rasqal_FOUND "Rasqal" "Rasqal Development Libraries" "fixme" FALSE "" "fixme")

if(Redland_FOUND AND Raptor_FOUND AND Rasqal_FOUND)
  add_subdirectory(src)
  add_subdirectory(devel)
  add_subdirectory(tests)
endif(Redland_FOUND AND Raptor_FOUND AND Rasqal_FOUND)
