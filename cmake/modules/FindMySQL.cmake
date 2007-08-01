# - Find MySQL
# Find the MySQL includes and client library
# This module defines
#  MYSQL_INCLUDE_DIR, where to find mysql.h
#  MYSQL_LIBRARIES, the libraries needed to use MySQL.
#  MYSQL_FOUND, If false, do not try to use MySQL.

find_file(MYSQL_CONFIG mysql_config)

if(MYSQL_CONFIG)
    set(MYSQL_FOUND TRUE)

    execute_process(COMMAND ${MYSQL_CONFIG} --include OUTPUT_VARIABLE MYSQL_INCLUDE_DIR)
    execute_process(COMMAND ${MYSQL_CONFIG} --libs OUTPUT_VARIABLE MYSQL_LIBRARIES)

    message(STATUS "MySQL found.")
    message(STATUS "MySQL include dir: ${MYSQL_INCLUDE_DIR}")
    message(STATUS "MySQL libs: ${MYSQL_LIBRARIES}")
else(MYSQL_CONFIG)
    set(MYSQL_FOUND FALSE)
    message(STATUS "MySQL not found.")
endif(MYSQL_CONFIG)
