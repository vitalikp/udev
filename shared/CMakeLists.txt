# udev shared cmake file 

# udev shared objects
add_library(libudev_shared_obj STATIC
	special.h
	device-nodes.c
	device-nodes.h
	util.c
	util.h
	virt.c
	virt.h
	architecture.c
	architecture.h
	path-util.c
	path-util.h
	time-util.c
	time-util.h
	hashmap.c
	hashmap.h
	siphash24.c
	siphash24.h
	set.c
	set.h
	prioq.c
	prioq.h
	strv.c
	strv.h
	env-util.c
	env-util.h
	strbuf.c
	strbuf.h
	strxcpyx.c
	strxcpyx.h
	conf-parser.c
	conf-parser.h
	log.c
	log.h
	utf8.c
	utf8.h
	gunicode.c
	gunicode.h
	conf-files.c
	conf-files.h
	cgroup-util.c
	cgroup-util.h
	unit-name.c
	unit-name.h
	fileio.c
	fileio.h
	MurmurHash2.c
	MurmurHash2.h
	mkdir.c
	mkdir.h
	smack-util.c
	smack-util.h
	apparmor-util.c
	apparmor-util.h
	ima-util.c
	ima-util.h
	condition-util.c
	condition-util.h
	bus-label.c
	bus-label.h
	# libsystemd-core
	condition.c
	# libsystemd-internal
	sd-event.c
	sd-rtnl.c
	rtnl-util.c
	rtnl-types.c
	rtnl-message.c
	sd-daemon.c
	sd-id128.c
	# libsystemd-network
	network-internal.c
)

add_library(libudev_label_obj STATIC
	label.c
	label.h
	selinux-util.c
	selinux-util.h
	mkdir-label.c
	dev-setup.c
	dev-setup.h
)
target_link_libraries(libudev_label_obj ${SELINUX_LIBRARIES} ${SMACK_LIBRARIES})
