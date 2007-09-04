project(soprano_backends)

if(REDLAND_FOUND)
  add_subdirectory(redland)
endif(REDLAND_FOUND)

if(TSTORE_FOUND)
  add_subdirectory(3store)
endif(TSTORE_FOUND)

if(JNI_1_6_FOUND)
  add_subdirectory(sesame2)
endif(JNI_1_6_FOUND)
