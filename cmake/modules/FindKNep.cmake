FIND_PATH(KNEP_INCLUDE_DIR knep/knep.h
   /usr/include
   /usr/local/include
   /home/kdedev/test/include
)

FIND_LIBRARY(KNEP_LIBRARIES NAMES knepclient
   PATHS
   /usr/lib
   /usr/local/lib
   /home/kdedev/test/lib  
)

if(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
   set(KNEP_FOUND TRUE)
   message(STATUS "Found KNepomuk: ${KNEP_LIBRARIES}")
endif(KNEP_INCLUDE_DIR AND KNEP_LIBRARIES)
