# - Try to find 3Store V3
# Once done this will define
#
#  TSTORE_FOUND       - system has 3Store
#  TSTORE_LIBRARIES   - Link these to use 3Store
#  TSTORE_CFLAGS      - Compiler flags

# (c) 2007 Sebastian Trueg <trueg@kde.org>
#
# Based on FindFontconfig Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (TSTORE_LIBRARIES AND TSTORE_INCLUDE_DIR AND TSTORE_CFLAGS)

  # in cache already
  message(STATUS "Found 3Store: ${TSTORE_LIBRARIES} [${TSTORE_INCLUDE_DIR}] (cached)")
  set(TSTORE_FOUND TRUE)

else (TSTORE_LIBRARIES AND TSTORE_INCLUDE_DIR AND TSTORE_CFLAGS)

  # 3store uses glib and mysql includes in the headers. BAD!
  find_package(GLIB2 REQUIRED)
  find_package(MySQL REQUIRED)

  set(TSTORE_CFLAGS "${GLIB2_CFLAGS} -I${GLIB2_INCLUDE_DIR} ${MYSQL_INCLUDE_DIR}")

  find_path(TSTORE_INCLUDE_DIR 3store3/tstore.h
    PATHS
    /usr/include
    /usr/local/include
    ${INCLUDE_INSTALL_DIR}
    )

  find_library(TSTORE_LIBRARIES NAMES 3store
    PATHS
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    ${LIB_INSTALL_DIR}
    )

  if (TSTORE_LIBRARIES AND TSTORE_INCLUDE_DIR)
     set(TSTORE_FOUND TRUE)
  endif (TSTORE_LIBRARIES AND TSTORE_INCLUDE_DIR)

  if (TSTORE_FOUND)
    if (NOT TStore_FIND_QUIETLY)
      message(STATUS "Found 3Store: ${TSTORE_LIBRARIES} [${TSTORE_INCLUDE_DIR}]")
    endif (NOT TStore_FIND_QUIETLY)
  else (TSTORE_FOUND)
    if (TStore_FIND_REQUIRED)
      message(STATUS "Could NOT find 3Store")
    endif (TStore_FIND_REQUIRED)
  endif (TSTORE_FOUND)

#  mark_as_advanced(TSTORE_LIBRARIES)

endif (TSTORE_LIBRARIES AND TSTORE_INCLUDE_DIR AND TSTORE_CFLAGS)
