// udev test
//
// Provides automated testing of the udev binary.
// The whole test is self contained in this file, except the matching sysfs tree.
// Simply extend the @tests array, to add a new test variant.
//
// Every test is driven by its own temporary config file.
// This program prepares the environment, creates the config and calls udev.
//
// udev parses the rules, looks at the provided sysfs and
// first creates and then removes the device node.
// After creation and removal the result is checked against the
// expected value and the result is printed.
//
// Copyright (C) 2004-2012 Kay Sievers <kay@vrfy.org>
// Copyright (C) 2004 Leann Ogasawara <ogasawara@osdl.org>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>


#define dev_num(major,minor) (minor&0xff)|((major&0xfff)<<8)|((minor&0xfff00)<<12)


typedef char * const perm_t[3];

typedef struct udev_test_t udev_test_t;

struct udev_test_t
{
	const char		*desc;
	const char		*devpath;
	const char		*exp_name;
	const char		*not_exp_name;
	const perm_t	exp_perms;
	const dev_t		exp_dev;
	const bool		exp_add_error;
	const bool		exp_rem_error;
	const bool		clean;
	const bool		keep;
	const char		*rules;
};


const int tests_len = 137;
const char* udev_bin = "./test-udev";
const char* udev_rules = "run/udev/rules.d/udev-test.rules";


const udev_test_t tests[] =
{
	{ "no rules",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/sda",
	  .exp_rem_error = true,
	  .rules = "#\n\0"
	},
	{ "label test of scsi disc",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/boot_disk",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", SYMLINK+=\"boot_disk%n\"\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "label test of scsi disc",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/boot_disk",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", SYMLINK+=\"boot_disk%n\"\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "label test of scsi disc",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/boot_disk",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", SYMLINK+=\"boot_disk%n\"\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "label test of scsi partition",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/boot_disk1",
	  .rules = "SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", SYMLINK+=\"boot_disk%n\"\n\0"
	},
	{ "label test of pattern match",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/boot_disk1",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"?ATA\", SYMLINK+=\"boot_disk%n-1\"\n\
SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA?\", SYMLINK+=\"boot_disk%n-2\"\n\
SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"A??\", SYMLINK+=\"boot_disk%n\"\n\
SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATAS\", SYMLINK+=\"boot_disk%n-3\"\n\0"
	},
	{ "label test of multiple sysfs files",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/boot_disk1",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", ATTRS{model}==\"ST910021AS X \", SYMLINK+=\"boot_diskX%n\"\n\
SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", ATTRS{model}==\"ST910021AS\", SYMLINK+=\"boot_disk%n\"\n\0"
	},
	{ "label test of max sysfs files (skip invalid rule)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/boot_disk1",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", ATTRS{model}==\"ST910021AS\", ATTRS{scsi_level}==\"6\", ATTRS{rev}==\"4.06\", ATTRS{type}==\"0\", ATTRS{queue_depth}==\"32\", SYMLINK+=\"boot_diskXX%n\"\n\
SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", ATTRS{model}==\"ST910021AS\", ATTRS{scsi_level}==\"6\", ATTRS{rev}==\"4.06\", ATTRS{type}==\"0\", SYMLINK+=\"boot_disk%n\"\n\0"
	},
	{ "catch device by *",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem/0",
	  .rules = "KERNEL==\"ttyACM*\", SYMLINK+=\"modem/%n\"\n\0"
	},
	{ "catch device by * - take 2",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem/0",
	  .rules =
"KERNEL==\"*ACM1\", SYMLINK+=\"bad\"\n\
KERNEL==\"*ACM0\", SYMLINK+=\"modem/%n\"\n\0"
	},
	{ "catch device by ?",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem/0",
	  .rules =
"KERNEL==\"ttyACM??*\", SYMLINK+=\"modem/%n-1\"\n\
KERNEL==\"ttyACM??\", SYMLINK+=\"modem/%n-2\"\n\
KERNEL==\"ttyACM?\", SYMLINK+=\"modem/%n\"\n\0"
	},
	{ "catch device by character class",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem/0",
	  .rules =
"KERNEL==\"ttyACM[A-Z]*\", SYMLINK+=\"modem/%n-1\"\n\
KERNEL==\"ttyACM?[0-9]\", SYMLINK+=\"modem/%n-2\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"modem/%n\"\n\0"
	},
	{ "replace kernel name",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules = "KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "Handle comment lines in config file (and replace kernel name)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules =
"# this is a comment\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\
\n\n\0"
	},
	{ "Handle comment lines in config file with whitespace (and replace kernel name)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules =
"# this is a comment with whitespace before the comment\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\n\0"
	},
	{ "Handle whitespace only lines (and replace kernel name)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/whitespace",
	  .rules =
"\n\n\n\
# this is a comment with whitespace before the comment\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"whitespace\"\n\
\n\n\n\0"
	},
	{ "Handle empty lines in config file (and replace kernel name)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules = "\nKERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\n\0"
	},
	{ "Handle backslashed multi lines in config file (and replace kernel name)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules =
"KERNEL==\"ttyACM0\", \\\n\
SYMLINK+=\"modem\"\n\n\0"
	},
	{ "preserve backslashes, if they are not for a newline",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/aaa",
	  .rules = "KERNEL==\"ttyACM0\", PROGRAM==\"/bin/echo -e \\101\", RESULT==\"A\", SYMLINK+=\"aaa\"\n\0"
	},
	{ "Handle stupid backslashed multi lines in config file (and replace kernel name)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules =
"\n\
#\n\
\\\n\n\
\\\n\n\
#\\\n\n\
KERNEL==\"ttyACM0\", \\\n\
	SYMLINK+=\"modem\"\n\n\0"
	},
	{ "subdirectory handling",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/sub/direct/ory/modem",
	  .rules = "KERNEL==\"ttyACM0\", SYMLINK+=\"sub/direct/ory/modem\"\n\0"
	},
	{ "parent device name match of scsi partition",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/first_disk5",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", SYMLINK+=\"first_disk%n\"\n\0"
	},
	{ "test substitution chars",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/Major:8:minor:5:kernelnumber:5:id:0:0:0:0",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", SYMLINK+=\"Major:%M:minor:%m:kernelnumber:%n:id:%b\"\n\0"
	},
	{ "import of shell-value returned from program",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node12345678",
	  .rules =
"SUBSYSTEMS==\"scsi\", IMPORT{program}=\"/bin/echo -e \' TEST_KEY=12345678\\n  TEST_key2=98765\'\", SYMLINK+=\"node$env{TEST_KEY}\"\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "sustitution of sysfs value (%s{file})",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/disk-ATA-sda",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{vendor}==\"ATA\", SYMLINK+=\"disk-%s{vendor}-%k\"\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "program result substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/special-device-5",
	  .not_exp_name = "not",
	  .rules =
"SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n special-device\", RESULT==\"-special-*\", SYMLINK+=\"not\"\n\
SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n special-device\", RESULT==\"special-*\", SYMLINK+=\"%c-%n\"\n\0"
	},
	{ "program result substitution (newline removal)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/newline_removed",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo test\", RESULT==\"test\", SYMLINK+=\"newline_removed\"\n\0"
	},
	{ "program result substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/test-0:0:0:0",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n test-%b\", RESULT==\"test-0:0*\", SYMLINK+=\"%c\"\n\0"
	},
	{ "program with lots of arguments",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/foo9",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n foo3 foo4 foo5 foo6 foo7 foo8 foo9\", KERNEL==\"sda5\", SYMLINK+=\"%c{7}\"\n\0"
	},
	{ "program with subshell",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/bar9",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/sh -c 'echo foo3 foo4 foo5 foo6 foo7 foo8 foo9 | sed  s/foo9/bar9/'\", KERNEL==\"sda5\", SYMLINK+=\"%c{7}\"\n\0"
	},
	{ "program arguments combined with apostrophes",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/foo7",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n 'foo3 foo4'   'foo5   foo6   foo7 foo8'\", KERNEL==\"sda5\", SYMLINK+=\"%c{5}\"\n\0"
	},
	{ "characters before the %c{N} substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/my-foo9",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n foo3 foo4 foo5 foo6 foo7 foo8 foo9\", KERNEL==\"sda5\", SYMLINK+=\"my-%c{7}\"\n\0"
	},
	{ "substitute the second to last argument",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/my-foo8",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n foo3 foo4 foo5 foo6 foo7 foo8 foo9\", KERNEL==\"sda5\", SYMLINK+=\"my-%c{6}\"\n\0"
	},
	{ "test substitution by variable name",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/Major:8-minor:5-kernelnumber:5-id:0:0:0:0",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", SYMLINK+=\"Major:$major-minor:$minor-kernelnumber:$number-id:$id\"\n\0"
	},
	{ "test substitution by variable name 2",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/Major:8-minor:5-kernelnumber:5-id:0:0:0:0",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", DEVPATH==\"*/sda/*\", SYMLINK+=\"Major:$major-minor:%m-kernelnumber:$number-id:$id\"\n\0"
	},
	{ "test substitution by variable name 3",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/850:0:0:05",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", DEVPATH==\"*/sda/*\", SYMLINK+=\"%M%m%b%n\"\n\0"
	},
	{ "test substitution by variable name 4",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/855",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", DEVPATH==\"*/sda/*\", SYMLINK+=\"$major$minor$number\"\n\0"
	},
	{ "test substitution by variable name 5",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/8550:0:0:0",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", DEVPATH==\"*/sda/*\", SYMLINK+=\"$major%m%n$id\"\n\0"
	},
	{ "non matching SUBSYSTEMS for device with no parent",
	  "/devices/virtual/tty/console", "dev/TTY",
	  .rules =
"SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n foo\", RESULT==\"foo\", SYMLINK+=\"foo\"\n\
KERNEL==\"console\", SYMLINK+=\"TTY\"\n\0"
	},
	{ "non matching SUBSYSTEMS",
	  "/devices/virtual/tty/console", "dev/TTY",
	  .rules =
"SUBSYSTEMS==\"foo\", ATTRS{dev}==\"5:1\", SYMLINK+=\"foo\"\n\
KERNEL==\"console\", SYMLINK+=\"TTY\"\n\0"
	},
	{ "ATTRS match",
	  "/devices/virtual/tty/console", "dev/foo",
	  .rules =
"KERNEL==\"console\", SYMLINK+=\"TTY\"\n\
ATTRS{dev}==\"5:1\", SYMLINK+=\"foo\"\n\0"
	},
	{ "ATTR (empty file)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/empty",
	  .rules =
"KERNEL==\"sda\", ATTR{test_empty_file}==\"?*\", SYMLINK+=\"something\"\n\
KERNEL==\"sda\", ATTR{test_empty_file}!=\"\", SYMLINK+=\"not-empty\"\n\
KERNEL==\"sda\", ATTR{test_empty_file}==\"\", SYMLINK+=\"empty\"\n\
KERNEL==\"sda\", ATTR{test_empty_file}!=\"?*\", SYMLINK+=\"not-something\"\n\0"
	},
	{ "ATTR (non-existent file)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/non-existent",
	  .rules =
"KERNEL==\"sda\", ATTR{nofile}==\"?*\", SYMLINK+=\"something\"\n\
KERNEL==\"sda\", ATTR{nofile}!=\"\", SYMLINK+=\"not-empty\"\n\
KERNEL==\"sda\", ATTR{nofile}==\"\", SYMLINK+=\"empty\"\n\
KERNEL==\"sda\", ATTR{nofile}!=\"?*\", SYMLINK+=\"not-something\"\n\
KERNEL==\"sda\", TEST!=\"nofile\", SYMLINK+=\"non-existent\"\n\
KERNEL==\"sda\", SYMLINK+=\"wrong\"\n\0"
	},
	{ "program and bus type match",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/scsi-0:0:0:0",
	  .rules =
"SUBSYSTEMS==\"usb\", PROGRAM==\"/bin/echo -n usb-%b\", SYMLINK+=\"%c\"\n\
SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n scsi-%b\", SYMLINK+=\"%c\"\n\
SUBSYSTEMS==\"foo\", PROGRAM==\"/bin/echo -n foo-%b\", SYMLINK+=\"%c\"\n\0"
	},
	{ "sysfs parent hierarchy",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem",
	  .rules = "ATTRS{idProduct}==\"007b\", SYMLINK+=\"modem\"\n\0"
	},
	{ "name test with ! in the name",
	  "/devices/virtual/block/fake!blockdev0", "dev/is/a/fake/blockdev0",
	  .rules =
"SUBSYSTEMS==\"scsi\", SYMLINK+=\"is/not/a/%k\"\n\
SUBSYSTEM==\"block\", SYMLINK+=\"is/a/%k\"\n\
KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "name test with ! in the name, but no matching rule",
	  "/devices/virtual/block/fake!blockdev0", "dev/fake/blockdev0",
	  .exp_rem_error = true,
	  .rules = "KERNEL==\"ttyACM0\", SYMLINK+=\"modem\"\n\0"
	},
	{ "KERNELS rule",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/scsi-0:0:0:0",
	  .rules =
"SUBSYSTEMS==\"usb\", KERNELS==\"0:0:0:0\", SYMLINK+=\"not-scsi\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:1\", SYMLINK+=\"no-match\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\":0\", SYMLINK+=\"short-id\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"/0:0:0:0\", SYMLINK+=\"no-match\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", SYMLINK+=\"scsi-0:0:0:0\"\n\0"
	},
	{ "KERNELS wildcard all",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/scsi-0:0:0:0",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNELS==\"*:1\", SYMLINK+=\"no-match\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"*:0:1\", SYMLINK+=\"no-match\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"*:0:0:1\", SYMLINK+=\"no-match\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"0:0:0:0\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"*\", SYMLINK+=\"scsi-0:0:0:0\"\n\0"
	},
	{ "KERNELS wildcard partial",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/scsi-0:0:0:0",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"*:0\", SYMLINK+=\"scsi-0:0:0:0\"\n\0"
	},
	{ "KERNELS wildcard partial 2",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/scsi-0:0:0:0",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNELS==\"0:0:0:0\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", KERNELS==\"*:0:0:0\", SYMLINK+=\"scsi-0:0:0:0\"\n\0"
	},
	{ "substitute attr with link target value (first match)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/driver-is-sd",
	  .rules = "SUBSYSTEMS==\"scsi\", SYMLINK+=\"driver-is-$attr{driver}\"\n\0"
	},
	{ "substitute attr with link target value (currently selected device)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/driver-is-ahci",
	  .rules = "SUBSYSTEMS==\"pci\", SYMLINK+=\"driver-is-$attr{driver}\"\n\0"
	},
	{ "ignore ATTRS attribute whitespace",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/ignored",
	  .rules = "SUBSYSTEMS==\"scsi\", ATTRS{whitespace_test}==\"WHITE  SPACE\", SYMLINK+=\"ignored\"\n\0"
	},
	{ "do not ignore ATTRS attribute whitespace",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/matched-with-space",
	  .rules =
"SUBSYSTEMS==\"scsi\", ATTRS{whitespace_test}==\"WHITE  SPACE \", SYMLINK+=\"wrong-to-ignore\"\n\
SUBSYSTEMS==\"scsi\", ATTRS{whitespace_test}==\"WHITE  SPACE   \", SYMLINK+=\"matched-with-space\"\n\0"
	},
	{ "permissions USER=bad GROUP=name",
	  "/devices/virtual/tty/tty33", "dev/tty33",
	  .exp_perms = {"0","0","0600"},
	  .rules = "KERNEL==\"tty33\", OWNER=\"bad\", GROUP=\"name\"\n\0"
	},
	{ "permissions OWNER=1",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = { "1",0,"0600"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", OWNER=\"1\"\n\0"
	},
	{ "permissions GROUP=1",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = {0,"1","0660"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", GROUP=\"1\"\n\0"
	},
	{ "textual user id",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = {"nobody",0,"0600"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", OWNER=\"nobody\"\n\0"
	},
	{ "textual group id",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = {0,"daemon","0660"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", GROUP=\"daemon\"\n\0"
	},
	{ "textual user/group id",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = {"root","mail","0660"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", OWNER=\"root\", GROUP=\"mail\"\n\0"
	},
	{ "permissions MODE=0777",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = {0,0,"0777"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", MODE=\"0777\"\n\0"
	},
	{ "permissions OWNER=1 GROUP=1 MODE=0777",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_perms = {"1","1","0777"},
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", OWNER=\"1\", GROUP=\"1\", MODE=\"0777\"\n\0"
	},
	{ "permissions OWNER to 1",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {"1",0,0},
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", OWNER=\"1\"\n\0"
	},
	{ "permissions GROUP to 1",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {0,"1","0660"},
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", GROUP=\"1\"\n\0"
	},
	{ "permissions MODE to 0060",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {0,0,"0060"},
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", MODE=\"0060\"\n\0"
	},
	{ "permissions OWNER, GROUP, MODE",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {"1","1","0777"},
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", OWNER=\"1\", GROUP=\"1\", MODE=\"0777\"\n\0"
	},
	{ "permissions only rule",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {"1","1","0777"},
	  .rules =
"KERNEL==\"ttyACM[0-9]*\", OWNER=\"1\", GROUP=\"1\", MODE=\"0777\"\n\
KERNEL==\"ttyUSX[0-9]*\", OWNER=\"2\", GROUP=\"2\", MODE=\"0444\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\"\n\0"
	},
	{ "multiple permissions only rule",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {"1","1","0777"}	,
	  .rules =
"SUBSYSTEM==\"tty\", OWNER=\"1\"\n\
SUBSYSTEM==\"tty\", GROUP=\"1\"\n\
SUBSYSTEM==\"tty\", MODE=\"0777\"\n\
KERNEL==\"ttyUSX[0-9]*\", OWNER=\"2\", GROUP=\"2\", MODE=\"0444\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\"\n\0"
	},
	{ "permissions only rule with override at SYMLINK+ rule",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/ttyACM0",
	  .exp_perms = {"1","2","0777"},
	  .rules =
"SUBSYSTEM==\"tty\", OWNER=\"1\"\n\
SUBSYSTEM==\"tty\", GROUP=\"2\"\n\
SUBSYSTEM==\"tty\", MODE=\"0777\"\n\
KERNEL==\"ttyUSX[0-9]*\", OWNER=\"2\", GROUP=\"2\", MODE=\"0444\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", GROUP=\"2\"\n\0"
	},
	{ "device number test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .exp_dev = dev_num(8,0),
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\"\n\0"
	},
	{ "big major number test",
	  "/devices/virtual/misc/misc-fake1", "dev/node",
	  .exp_dev = dev_num(4095,1),
	  .rules = "KERNEL==\"misc-fake1\", SYMLINK+=\"node\"\n\0"
	},
	{ "big major and big minor number test",
	  "/devices/virtual/misc/misc-fake89999", "dev/node",
	  .exp_dev = dev_num(4095,89999),
	  .rules = "KERNEL==\"misc-fake89999\", SYMLINK+=\"node\"\n\0"
	},
	{ "multiple symlinks with format char",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/symlink2-ttyACM0",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK=\"symlink1-%n symlink2-%k symlink3-%b\"\n\0"
	},
	{ "multiple symlinks with a lot of s p a c e s",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/one",
	  .not_exp_name = " ",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK=\"  one     two        \"\n\0"
	},
	{ "symlink creation (same directory)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/modem0",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", SYMLINK=\"modem%n\"\n\0"
	},
	{ "multiple symlinks",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/second-0",
	  .rules = "KERNEL==\"ttyACM0\", SYMLINK=\"first-%n second-%n third-%n\"\n\0"
	},
	{ "symlink name '.'",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/.",
	  .exp_add_error = true, .exp_rem_error = true,
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\".\"\n\0"
	},
	{ "symlink node to itself",
	  "/devices/virtual/tty/tty0", "dev/link",
	  .exp_add_error = true, .exp_rem_error = true, .clean = true,
	  .rules = "KERNEL==\"tty0\", SYMLINK+=\"tty0\"\n\0"
	},
	{ "symlink %n substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/symlink0",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", SYMLINK+=\"symlink%n\"\n\0"
	},
	{ "symlink %k substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/symlink-ttyACM0",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", SYMLINK+=\"symlink-%k\"\n\0"
	},
	{ "symlink %M:%m substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/major-166:0",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"ttyACM%n\", SYMLINK+=\"major-%M:%m\"\n\0"
	},
	{ "symlink %b substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/symlink-0:0:0:0",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"symlink-%b\"\n\0"
	},
	{ "symlink %c substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/test",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", PROGRAM==\"/bin/echo test\", SYMLINK+=\"%c\"\n\0"
	},
	{ "symlink %c{N} substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/test",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", PROGRAM==\"/bin/echo symlink test this\", SYMLINK+=\"%c{2}\"\n\0"
	},
	{ "symlink %c{N+} substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/this",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", PROGRAM==\"/bin/echo symlink test this\", SYMLINK+=\"%c{2+}\"\n\0"
	},
	{ "symlink only rule with %c{N+}",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/test",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", PROGRAM==\"/bin/echo link test this\" SYMLINK+=\"%c{2+}\"\n\0"
	},
	{ "symlink %s{filename} substitution",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/166:0",
	  .rules = "KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"%s{dev}\"\n\0"
	},
	{ "program result substitution (numbered part of)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/link1",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n node link1 link2\", RESULT==\"node *\", SYMLINK+=\"%c{2} %c{3}\"\n\0"
	},
	{ "program result substitution (numbered part of+)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda5", "dev/link4",
	  .rules = "SUBSYSTEMS==\"scsi\", PROGRAM==\"/bin/echo -n node link1 link2 link3 link4\", RESULT==\"node *\", SYMLINK+=\"%c{2+}\"\n\0"
	},
	{ "SUBSYSTEM match test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"should_not_match\", SUBSYSTEM==\"vc\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", SUBSYSTEM==\"block\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"should_not_match2\", SUBSYSTEM==\"vc\"\n\0"
	},
	{ "DRIVERS match test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"should_not_match\", DRIVERS==\"sd-wrong\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", SYMLINK+=\"node\", DRIVERS==\"sd\"\n\0"
	},
	{ "devnode substitution test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda\", PROGRAM==\"/usr/bin/test -b %N\" SYMLINK+=\"node\"\n\0"
	},
	{ "parent node name substitution test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/sda-part-1",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"%P-part-1\"\n\0"
	},
	{ "udev_root substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/start-/dev-end",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"start-%r-end\"\n\0"
	},
	{ "last_rule option",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/last",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"last\", OPTIONS=\"last_rule\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"very-last\"\n\0"
	},
	{ "negation KERNEL!=",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/match",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL!=\"sda1\", SYMLINK+=\"matches-but-is-negated\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", KERNEL!=\"xsda1\", SYMLINK+=\"match\"\n\0"
	},
	{ "negation SUBSYSTEM!=",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/not-anything",
	  .rules =
"SUBSYSTEMS==\"scsi\", SUBSYSTEM==\"block\", KERNEL!=\"sda1\", SYMLINK+=\"matches-but-is-negated\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", SUBSYSTEM!=\"anything\", SYMLINK+=\"not-anything\"\n\0"
	},
	{ "negation PROGRAM!= exit code",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/nonzero-program",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"before\"\n\
KERNEL==\"sda1\", PROGRAM!=\"/bin/false\", SYMLINK+=\"nonzero-program\"\n\0"
	},
	{ "ENV{} test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/true",
	  .rules =
"ENV{ENV_KEY_TEST}=\"test\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"go\", SYMLINK+=\"wrong\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"test\", SYMLINK+=\"true\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"bad\", SYMLINK+=\"bad\"\n\0"
	},
	{ "ENV{} test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/true",
	  .rules =
"ENV{ENV_KEY_TEST}=\"test\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"go\", SYMLINK+=\"wrong\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"yes\", ENV{ACTION}==\"add\", ENV{DEVPATH}==\"*/block/sda/sdax1\", SYMLINK+=\"no\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"test\", ENV{ACTION}==\"add\", ENV{DEVPATH}==\"*/block/sda/sda1\", SYMLINK+=\"true\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ENV_KEY_TEST}==\"bad\", SYMLINK+=\"bad\"\n\0"
	},
	{ "ENV{} test (assign)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/true",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}=\"true\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}==\"yes\", SYMLINK+=\"no\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}==\"true\", SYMLINK+=\"true\"\n\0"
	},
	{ "ENV{} test (assign 2 times)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/true",
	  .rules =
"SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}=\"true\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}=\"absolutely-$env{ASSIGN}\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", SYMLINK+=\"before\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}==\"yes\", SYMLINK+=\"no\"\n\
SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", ENV{ASSIGN}==\"absolutely-true\", SYMLINK+=\"true\"\n\0"
	},
	{ "ENV{} test (assign2)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/part",
	  .rules =
"SUBSYSTEM==\"block\", KERNEL==\"*[0-9]\", ENV{PARTITION}=\"true\", ENV{MAINDEVICE}=\"false\"\n\
SUBSYSTEM==\"block\", KERNEL==\"*[!0-9]\", ENV{PARTITION}=\"false\", ENV{MAINDEVICE}=\"true\"\n\
ENV{MAINDEVICE}==\"true\", SYMLINK+=\"disk\"\n\
SUBSYSTEM==\"block\", SYMLINK+=\"before\"\n\
ENV{PARTITION}==\"true\", SYMLINK+=\"part\"\n\0"
	},
	{ "untrusted string sanitize",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/sane",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", PROGRAM==\"/bin/echo -e name; (/usr/bin/badprogram)\", RESULT==\"name_ _/usr/bin/badprogram_\", SYMLINK+=\"sane\"\n\0"
	},
	{ "untrusted string sanitize (don't replace utf8)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/uber",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", PROGRAM==\"/bin/echo -e \\xc3\\xbcber\" RESULT==\"über\", SYMLINK+=\"uber\"\n\0"
	},
	{ "untrusted string sanitize (replace invalid utf8)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/replaced",
	  .rules = "SUBSYSTEMS==\"scsi\", KERNEL==\"sda1\", PROGRAM==\"/bin/echo -e \\xef\\xe8garbage\", RESULT==\"__garbage\", SYMLINK+=\"replaced\"\n\0"
	},
	{ "read sysfs value from parent device",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/serial-354172020305000",
	  .rules = "KERNEL==\"ttyACM*\", ATTRS{serial}==\"?*\", SYMLINK+=\"serial-%s{serial}\"\n\0"
	},
	{ "match against empty key string",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/ok",
	  .rules =
"KERNEL==\"sda\", ATTRS{nothing}!=\"\", SYMLINK+=\"not-1-ok\"\n\
KERNEL==\"sda\", ATTRS{nothing}==\"\", SYMLINK+=\"not-2-ok\"\n\
KERNEL==\"sda\", ATTRS{vendor}!=\"\", SYMLINK+=\"ok\"\n\
KERNEL==\"sda\", ATTRS{vendor}==\"\", SYMLINK+=\"not-3-ok\"\n\0"
	},
	{ "check ACTION value",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/ok",
	  .rules =
"ACTION==\"unknown\", KERNEL==\"sda\", SYMLINK+=\"unknown-not-ok\"\n\
ACTION==\"add\", KERNEL==\"sda\", SYMLINK+=\"ok\"\n\0"
	},
	{ "final assignment",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/ok",
	  .exp_perms = {"root","tty","0640"},
	  .rules =
"KERNEL==\"sda\", GROUP:=\"tty\"\n\
KERNEL==\"sda\", GROUP=\"not-ok\", MODE=\"0640\", SYMLINK+=\"ok\"\n\0"
	},
	{ "final assignment 2",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/ok",
	  .exp_perms = {"root","tty","0640"},
	  .rules =
"KERNEL==\"sda\", GROUP:=\"tty\"\n\
SUBSYSTEM==\"block\", MODE:=\"640\"\n\
KERNEL==\"sda\", GROUP=\"not-ok\", MODE=\"0666\", SYMLINK+=\"ok\"\n\0"
	},
	{ "env substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/node-add-me",
	  .rules = "KERNEL==\"sda\", MODE=\"0666\", SYMLINK+=\"node-$env{ACTION}-me\"\n\0"
	},
	{ "reset list to current value",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/three",
	  .not_exp_name  = "two",
	  .rules =
"KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"one\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"two\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK=\"three\"\n\0"
	},
	{ "test empty SYMLINK+ (empty override)",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/right",
	  .not_exp_name = "wrong",
	  .rules =
"KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"wrong\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK=\"\"\n\
KERNEL==\"ttyACM[0-9]*\", SYMLINK+=\"right\"\n\0"
	},
	{ "test multi matches",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/right",
	  .rules =
"KERNEL==\"ttyACM*\", SYMLINK+=\"before\"\n\
KERNEL==\"ttyACM*|nothing\", SYMLINK+=\"right\"\n\0"
	},
	{ "test multi matches 2",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/right",
	  .rules =
"KERNEL==\"dontknow*|*nothing\", SYMLINK+=\"nomatch\"\n\
KERNEL==\"ttyACM*\", SYMLINK+=\"before\"\n\
KERNEL==\"dontknow*|ttyACM*|nothing*\", SYMLINK+=\"right\"\n\0"
	},
	{ "test multi matches 3",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/right",
	  .rules =
"KERNEL==\"dontknow|nothing\", SYMLINK+=\"nomatch\"\n\
KERNEL==\"dontknow|ttyACM0a|nothing|attyACM0\", SYMLINK+=\"wrong1\"\n\
KERNEL==\"X|attyACM0|dontknow|ttyACM0a|nothing|attyACM0\", SYMLINK+=\"wrong2\"\n\
KERNEL==\"dontknow|ttyACM0|nothing\", SYMLINK+=\"right\"\n\0"
	},
	{ "test multi matches 4",
	  "/devices/pci0000:00/0000:00:1d.7/usb5/5-2/5-2:1.0/tty/ttyACM0", "dev/right",
	  .rules =
"KERNEL==\"dontknow|nothing\", SYMLINK+=\"nomatch\"\n\
KERNEL==\"dontknow|ttyACM0a|nothing|attyACM0\", SYMLINK+=\"wrong1\"\n\
KERNEL==\"X|attyACM0|dontknow|ttyACM0a|nothing|attyACM0\", SYMLINK+=\"wrong2\"\n\
KERNEL==\"all|dontknow|ttyACM0\", SYMLINK+=\"right\"\n\
KERNEL==\"ttyACM0a|nothing\", SYMLINK+=\"wrong3\"\n\0"
	},
	{ "IMPORT parent test sequence 1/2 (keep)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/parent",
	  .keep = true,
	  .rules =
"KERNEL==\"sda\", IMPORT{program}=\"/bin/echo -e \'PARENT_KEY=parent_right\\nWRONG_PARENT_KEY=parent_wrong'\"\n\
KERNEL==\"sda\", SYMLINK+=\"parent\"\n\0"
	},
	{ "IMPORT parent test sequence 2/2 (keep)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/parentenv-parent_right",
	  .clean = true,
	  .rules = "KERNEL==\"sda1\", IMPORT{parent}=\"PARENT*\", SYMLINK+=\"parentenv-$env{PARENT_KEY}$env{WRONG_PARENT_KEY}\"\n\0"
	},
	{ "GOTO test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/right",
	  .rules =
"KERNEL==\"sda1\", GOTO=\"TEST\"\n\
KERNEL==\"sda1\", SYMLINK+=\"wrong\"\n\
KERNEL==\"sda1\", GOTO=\"BAD\"\n\
KERNEL==\"sda1\", SYMLINK+=\"\", LABEL=\"NO\"\n\
KERNEL==\"sda1\", SYMLINK+=\"right\", LABEL=\"TEST\", GOTO=\"end\"\n\
KERNEL==\"sda1\", SYMLINK+=\"wrong2\", LABEL=\"BAD\"\n\
LABEL=\"end\"\n\0"
	},
	{ "GOTO label does not exist",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/right",
	  .rules =
"KERNEL==\"sda1\", GOTO=\"does-not-exist\"\n\
KERNEL==\"sda1\", SYMLINK+=\"right\",\n\
LABEL=\"exists\"\n\0"
	},
	{ "SYMLINK+ compare test",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/right",
	  .not_exp_name = "wrong",
	  .rules =
"KERNEL==\"sda1\", SYMLINK+=\"link\"\n\
KERNEL==\"sda1\", SYMLINK==\"link*\", SYMLINK+=\"right\"\n\
KERNEL==\"sda1\", SYMLINK==\"nolink*\", SYMLINK+=\"wrong\"\n\0"
	},
	{ "invalid key operation",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/yes",
	  .rules =
"KERNEL=\"sda1\", SYMLINK+=\"no\"\n\
KERNEL==\"sda1\", SYMLINK+=\"yes\"\n\0"
	},
	{ "operator chars in attribute",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/yes",
	  .rules = "KERNEL==\"sda\", ATTR{test:colon+plus}==\"?*\", SYMLINK+=\"yes\"\n\0"
	},
	{ "overlong comment line",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda/sda1", "dev/yes",
	  .rules =
"# 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\n\
# 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\n\
KERNEL==\"sda1\", SYMLINK+==\"no\"\n\
KERNEL==\"sda1\", SYMLINK+=\"yes\"\n\0"
	},
	{ "magic subsys/kernel lookup",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/00:16:41:e2:8d:ff",
	  .rules = "KERNEL==\"sda\", SYMLINK+=\"$attr{[net/eth0]address}\"\n\0"
	},
	{ "TEST absolute path",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/there",
	  .rules =
"TEST==\"/etc/hosts\", SYMLINK+=\"there\"\n\
TEST!=\"/etc/hosts\", SYMLINK+=\"notthere\"\n\0"
	},
	{ "TEST subsys/kernel lookup",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/yes",
	  .rules = "KERNEL==\"sda\", TEST==\"[net/eth0]\", SYMLINK+=\"yes\"\n\0"
	},
	{ "TEST relative path",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/relative",
	  .rules = "KERNEL==\"sda\", TEST==\"size\", SYMLINK+=\"relative\"\n\0"
	},
	{ "TEST wildcard substitution (find queue/nr_requests)",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/found-subdir",
	  .rules = "KERNEL==\"sda\", TEST==\"*/nr_requests\", SYMLINK+=\"found-subdir\"\n\0"
	},
	{ "TEST MODE=0000",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/sda",
	  .exp_perms = {"0","0","0000"},
	  .exp_rem_error = true,
	  .rules = "KERNEL==\"sda\", MODE=\"0000\"\n\0"
	},
	{ "TEST PROGRAM feeds OWNER, GROUP, MODE",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/sda",
	  .exp_perms = {"1","1","0400"},
	  .exp_rem_error = true,
	  .rules =
"KERNEL==\"sda\", MODE=\"666\"\n\
KERNEL==\"sda\", PROGRAM==\"/bin/echo 1 1 0400\", OWNER=\"%c{1}\", GROUP=\"%c{2}\", MODE=\"%c{3}\"\n\0"
	},
	{ "TEST PROGRAM feeds MODE with overflow",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/sda",
	  .exp_perms = {"0","0","0440"},
	  .exp_rem_error = true,
	  .rules =
"KERNEL==\"sda\", MODE=\"440\"\n\
KERNEL==\"sda\", PROGRAM==\"/bin/echo 0 0 0400letsdoabuffferoverflow0123456789012345789012345678901234567890\", OWNER=\"%c{1}\", GROUP=\"%c{2}\", MODE=\"%c{3}\"\n\0"
	},
	{ "magic [subsys/sysname] attribute substitution",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/sda-8741C4G-end",
	  .exp_perms = {"0","0","0600"},
	  .rules =
"KERNEL==\"sda\", PROGRAM=\"/bin/true create-envp\"\n\
KERNEL==\"sda\", ENV{TESTENV}=\"change-envp\"\n\
KERNEL==\"sda\", SYMLINK+=\"%k-%s{[dmi/id]product_name}-end\"\n\0"
	},
	{ "builtin path_id",
	  "/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/block/sda", "dev/disk/by-path/pci-0000:00:1f.2-scsi-0:0:0:0",
	  .rules =
"KERNEL==\"sda\", IMPORT{builtin}=\"path_id\"\n\
KERNEL==\"sda\", ENV{ID_PATH}==\"?*\", SYMLINK+=\"disk/by-path/$env{ID_PATH}\"\n\0"
	}
};

int error = 0;

int udev(const char *action, const char *devpath, const char *rules)
{
	// create temporary rules
	unsigned char *p = (char*)rules;
	FILE *conf = fopen(udev_rules, "wb");
	if (!conf)
	{
		printf("unable to create rules file: %s\n", udev_rules);
		exit(EXIT_FAILURE);
	}
	while (*p != '\0')
		fwrite(p++, 1, 1, conf);
	fclose(conf);

	char cmd[512];
	sprintf(cmd, "%s %s %s", udev_bin, action, devpath);
	return system(cmd);
}

void permissions_test(const perm_t perm, uid_t uid, gid_t gid, mode_t mode)
{
	int wrong = 0;
	uid_t userid;
	gid_t groupid;
	mode_t modeid;

	if (perm[0])
	{
		struct passwd *pw = getpwnam(perm[0]);
		if (pw)
			userid = pw->pw_uid;
		else
			userid = strtod(perm[0], NULL);
		if (uid != userid)
			wrong = 1;
	}

	if (perm[1])
	{
		struct group *gr = getgrnam(perm[1]);
		if (gr)
			groupid = gr->gr_gid;
		else
			groupid = strtod(perm[1], NULL);
		if (gid != groupid)
			wrong = 1;
	}

	if (perm[2])
	{
		modeid = strtol(perm[2], NULL, 8);
		if ((mode & 07777) != modeid)
			wrong = 1;
	}

	if (!wrong)
		printf("permissions: ok\n");
	else
	{
		printf("  expected permissions are: %i:%i:%#o\n", userid, groupid, modeid);
		printf("  created permissions are : %i:%i:%#o\n", uid, gid, mode & 07777);
		printf("permissions: error\n");
		error++;
		sleep(1);
	}
}

void device_number_test(dev_t dev, dev_t rdev)
{
	if (dev == rdev)
		printf("device number: ok\n");
	else
	{
		printf("  expected device number is: %.4xh\n", dev);
		printf("  created device number is : %.4xh\n", rdev);
		printf("device number: error\n");
		error++;
		sleep(1);
	}
}

void udev_setup(void)
{
	system("rm -rf dev");
	if (mkdir("dev", 0777) < 0)
	{
		printf("unable to create 'dev': %m\n");
		exit(EXIT_FAILURE);
	}

	// setting group and mode of test/dev ensures the tests work
	// even if the parent directory has setgid bit enabled.
	if (chown("dev", 0, 0) < 0)
	{
		printf("unable to chown 'dev': %m\n");
		exit(EXIT_FAILURE);
	}

	if (chmod("dev", 0755) < 0)
	{
		printf("unable to chmod 'dev': %m\n");
		exit(EXIT_FAILURE);
	}

	system("rm -rf run");
	system("mkdir -p run/udev/rules.d");
}

void run_test(const udev_test_t *rules, int number)
{
	int res;

	printf("TEST %d: %s\n", number, rules->desc);
	printf("device '%s' expecting node/link '%s'\n", rules->devpath, rules->exp_name);

	res = udev("add", rules->devpath, rules->rules);
	if (res)
	{
		printf("%s add failed with code %d\n", udev_bin, res);
		error++;
	}

	struct stat st;
	int r;
	if (rules->not_exp_name)
	{
		r = stat(rules->not_exp_name, &st);
		if (!(r < 0 && errno == ENOENT) || S_ISLNK(st.st_mode) == 1)
		{
			printf("nonexistent: error '%s' not expected to be there\n", rules->not_exp_name);
			error++;
			sleep(1);
		}
	}

	r = stat(rules->exp_name, &st);
	if (r < 0 && errno == ENOENT && S_ISLNK(st.st_mode) != 1)
	{
		printf("add:         error");
		if (rules->exp_add_error)
			printf(" as expected\n");
		else
		{
			printf("\n");
			system("tree dev");
			error++;
			printf("\n");
			sleep(1);
		}
	}
	else
	{
		if ((rules->exp_perms[0])||(rules->exp_perms[1])||(rules->exp_perms[2]))
			permissions_test(rules->exp_perms, st.st_uid, st.st_gid, st.st_mode);
		if (rules->exp_dev)
			device_number_test(rules->exp_dev, st.st_rdev);
		printf("add:         ok\n");
	}

	if (rules->keep)
	{
		printf("\n\n");
		return;
	}

	res = udev("remove", rules->devpath, rules->rules);
	if (res)
	{
		printf("%s remove failed with code %d\n", udev_bin, res);
		error++;
	}

	r = stat(rules->exp_name, &st);
	if (r < 0 && errno == ENOENT && S_ISLNK(st.st_mode) != 1)
		printf("remove:      ok\n");
	else
	{
		printf("remove:      error");
		if (rules->exp_rem_error)
			printf(" as expected\n");
		else
		{
			printf("\n");
			system("tree dev");
			error++;
			printf("\n");
			sleep(1);
		}
	}

	printf("\n");

	if (rules->clean)
		udev_setup();
}

int main(int argc, char *argv[])
{
	// only run if we have root permissions
	// due to mknod restrictions
	if (getuid() != 0)
	{
		printf("Must have root permissions to run properly.\n");
		return EXIT_FAILURE;
	}
	setbuf(stdout, NULL);

	udev_setup();

	int test_num = 1;

	if (argc > 1)
	{
		int i = 1;
		while (i < argc)
		{
			test_num = strtod(argv[i], NULL);
			if (test_num > 0 && test_num <= tests_len)
				run_test(&tests[test_num-1], test_num);
			else
				printf("test does not exist.\n");
			i++;
		}
	}
	else
	{
		// test all
		printf("\nudev-test will run %d tests:\n\n", tests_len);
		while (test_num <= tests_len)
		{
			run_test(&tests[test_num-1], test_num);
			test_num++;
		}
	}

	printf("%d errors occurred\n\n", error);

	// cleanup
	system("rm -rf dev");
	system("rm -rf run");

	if (error > 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
