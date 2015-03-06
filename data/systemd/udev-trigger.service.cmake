#  This file is part of systemd.
#
#  systemd is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2.1 of the License, or
#  (at your option) any later version.

[Unit]
Description=udev Coldplug all Devices
Documentation=man:udev(7) man:udevd(8)
DefaultDependencies=no
Wants=udevd.service
After=udevd-kernel.socket udevd-control.socket
Before=sysinit.target
ConditionPathIsReadWrite=/sys

[Service]
Type=oneshot
RemainAfterExit=yes
ExecStart=@CMAKE_INSTALL_PREFIX@/bin/udevadm trigger --type=subsystems --action=add ; @CMAKE_INSTALL_PREFIX@/bin/udevadm trigger --type=devices --action=add
