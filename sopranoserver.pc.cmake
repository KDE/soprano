prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_DESTINATION@
includedir=@INCLUDE_INSTALL_DIR@

Name: Sopranoserver
Description: Soprano's RDF server implementation
URL=http://soprano.sourceforge.net/
Requires: QtCore
Version: @CMAKE_SOPRANO_VERSION_STRING@
Libs: -L@LIB_DESTINATION@ -lsopranoserver
Cflags: -I@INCLUDE_INSTALL_DIR@ -@INCLUDE_INSTALL_DIR@/soprano
