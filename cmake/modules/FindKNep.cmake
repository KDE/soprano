FIND_PATH(KNEP_INCLUDE_DIR knep/knep.h
   /usr/include
   /usr/local/include
   /home/kdedev/test/include
   ${KDE4_INCLUDE_DIR}
   ${INCLUDE_INSTALL_DIR}
)

FIND_LIBRARY(KNEP_LIBRARIES NAMES knepclient
   PATHS
   /usr/lib
   /usr/local/lib
   /home/kdedev/test/lib
   ${KDE4_LIB_DIR}
   ${LIB_INSTALL_DIR}
)

if(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
   set(KNEP_FOUND TRUE)
   message(STATUS "Found KNepomuk: ${KNEP_LIBRARIES}")
endif(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
