# udev
fork udev from [systemd 214](https://github.com/vitalikp/systemd)

# Install
 - download [udev](https://github.com/vitalikp/udev/archive/master.tar.gz);
 - unpack archive and go into directory;
 - create **build** directory;
 - run commands:
```
	# cd build
	# cmake ..
	# make
	# sudo make install
```
Use `cmake -D(OPTION) ..` instead of `cmake ..` for add config option.
##### Optional Features:
 - Use **SELINUX_ENABLE=OFF** option to disable SELINUX support(default=ON);

# Update
After update perform the following steps:
 - run `<bindir>/udevadm hwdb --update` for update hwdb;

# License
Original forked code is LGPLv2.1+.<br/>
New code is MIT License. See LICENSE file.
