# RPM macros for packages installing udev files

%_udevlibexecdir @udevlibexecdir@

%_udevhwdbdir @udevhwdbdir@

%udev_hwdb_update() \
udevadm hwdb --update >/dev/null 2>&1 || : \
%{nil}

%udev_rules_update() \
udevadm control --reload >/dev/null 2>&1 || : \
%{nil}