prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@INCLUDE_INSTALL_DIR@

Name: Sopranoclient
Description: Client for Soprano
URL=http://soprano.sourceforge.net/
Requires: QtCore
Version: @SOPRANO_VERSION_STRING@
Libs: -L${libdir} -lsopranoclient
Cflags: -I${includedir} -I${includedir}/soprano
