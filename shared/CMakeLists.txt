# udev shared cmake file 

# udev shared objects
add_library(udev_shared_obj STATIC
	ioprio.h
	missing.h
	special.h
	list.h
	macro.h
	def.h
	sparse-endian.h
	refcnt.h
	device-nodes.c
	device-nodes.h
	util.c
	util.h
	virt.c
	virt.h
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
	fileio.c
	fileio.h
	MurmurHash2.c
	MurmurHash2.h
	acpi-fpdt.h
	acpi-fpdt.c
	mkdir.c
	mkdir.h
	smack-util.c
	smack-util.h
	apparmor-util.c
	apparmor-util.h
	ima-util.c
	ima-util.h
	bus-label.c
	bus-label.h
)

add_library(udev_label_obj STATIC
	label.c
	label.h
	mkdir-label.c
	dev-setup.c
	dev-setup.h
)
target_link_libraries(udev_label_obj ${SELINUX_LIBRARIES})
