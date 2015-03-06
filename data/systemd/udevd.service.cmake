#  This file is part of systemd.
#
#  systemd is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2.1 of the License, or
#  (at your option) any later version.

[Unit]
Description=udev Kernel Device Manager
Documentation=man:udevd(8) man:udev(7)
DefaultDependencies=no
Wants=udevd-control.socket udevd-kernel.socket
After=udevd-control.socket udevd-kernel.socket
Before=sysinit.target
ConditionPathIsReadWrite=/sys

[Service]
Type=notify
OOMScoreAdjust=-1000
Sockets=udevd-control.socket udevd-kernel.socket
Restart=always
RestartSec=0
ExecStart=@CMAKE_INSTALL_PREFIX@/sbin/udevd
MountFlags=slave
