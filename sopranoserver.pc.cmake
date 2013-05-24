prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@INCLUDE_INSTALL_DIR@

Name: Sopranoserver
Description: Soprano's RDF server implementation
URL=http://soprano.sourceforge.net/
Requires: QtCore
Version: @SOPRANO_VERSION_STRING@
Libs: -L${libdir} -lsopranoserver
Cflags: -I${includedir} -I${includedir}/soprano
