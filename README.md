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

# Update
After update perform the following steps:
 - run `<bindir>/udevadm hwdb --update` for update hwdb;

# License
Original forked code is LGPLv2.1+.<br/>
New code is MIT License. See LICENSE file.
