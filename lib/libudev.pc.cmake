#  This file is part of systemd.
#
#  systemd is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2.1 of the License, or
#  (at your option) any later version.

prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@libdir@
includedir=@includedir@

Name: libudev
Description: Library to access udev device information
Version: @VERSION@
Libs: -L${libdir} -ludev
Cflags: -I${includedir}
