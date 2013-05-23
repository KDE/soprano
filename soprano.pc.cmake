prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@INCLUDE_INSTALL_DIR@

Name: Soprano
Description: Powerful RDF storage, parsing, and serialization framework
URL=http://soprano.sourceforge.net/
Requires: QtCore
Version: @CMAKE_SOPRANO_VERSION_STRING@
Libs: -L@LIB_INSTALL_DIR@ -lsoprano
Cflags: -I@INCLUDE_INSTALL_DIR@
