#  This file is part of systemd.
#
#  systemd is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2.1 of the License, or
#  (at your option) any later version.

[Unit]
Description=Cleanup udevd DB
DefaultDependencies=no
ConditionPathExists=/etc/initrd-release
Conflicts=udevd.service udevd-control.socket udevd-kernel.socket
After=udevd.service udevd-control.socket udevd-kernel.socket
Before=initrd-switch-root.target

[Service]
Type=oneshot
ExecStart=-@bindir@/udevadm info --cleanup-db
