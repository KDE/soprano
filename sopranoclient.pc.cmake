prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@INCLUDE_INSTALL_DIR@

Name: Sopranoclient
Description: Client for Soprano
URL=http://soprano.sourceforge.net/
Requires: QtCore
Version: @CMAKE_SOPRANO_VERSION_STRING@
Libs: -L@LIB_INSTALL_DIR@ -lsopranoclient
Cflags: -I@INCLUDE_INSTALL_DIR@ -@INCLUDE_INSTALL_DIR@/soprano
