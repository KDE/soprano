FIND_PATH(KNEP_INCLUDE_DIR knep/knep.h
   /usr/include
   /usr/local/include
   ${KDE4_INCLUDE_DIR}
   ${INCLUDE_INSTALL_DIR}
)

FIND_LIBRARY(KNEP_LIBRARIES NAMES knepclient
   PATHS
   /usr/lib
   /usr/local/lib
   ${KDE4_LIB_DIR}
   ${LIB_INSTALL_DIR}
)

if(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
   set(KNep_FOUND TRUE)
   message(STATUS "Found KNepomuk: ${KNEP_LIBRARIES}")
else(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
  if(KNep_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find KNepCLient library.")
  endif(KNep_FIND_REQUIRED)
endif(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
