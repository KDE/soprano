project(soprano_backends)

if(REDLAND_FOUND)
  add_subdirectory(redland)
endif(REDLAND_FOUND)

if(JNI_1_4_FOUND)
  add_subdirectory(sesame2)
endif(JNI_1_4_FOUND)

if(IODBC_FOUND)
  add_subdirectory(virtuoso)
endif(IODBC_FOUND)
