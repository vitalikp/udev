#  This file is part of systemd.
#
#  systemd is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2.1 of the License, or
#  (at your option) any later version.

# This service can dynamically be pulled-in by legacy services which
# cannot reliably cope with dynamic device configurations, and wrongfully
# expect a populated /dev during bootup.

[Unit]
Description=udev Wait for Complete Device Initialization
Documentation=man:udev(7) man:udevd(8)
DefaultDependencies=no
Wants=udevd.service
After=udev-trigger.service
Before=sysinit.target
ConditionPathIsReadWrite=/sys

[Service]
Type=oneshot
TimeoutSec=180
RemainAfterExit=yes
ExecStart=@CMAKE_INSTALL_PREFIX@/bin/udevadm settle
