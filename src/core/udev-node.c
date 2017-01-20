/*
 * Copyright (C) 2003-2013 Kay Sievers <kay@vrfy.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "udev.h"


static int symlink_create(const char *target, const char *slink)
{
	int err = 0;

	do {
		err = mkdir_parents_label(slink, 0755);
		if (err != 0 && err != -ENOENT)
				break;
		label_context_set(slink, S_IFLNK);
		err = symlink(target, slink);
		if (err != 0)
				err = -errno;
		label_context_clear();
	} while (err == -ENOENT);

	return err;
}

static int node_symlink(const char *devnum, const char *node, const char *slink)
{
        struct stat stats;
        char target[UTIL_PATH_SIZE];
        char slink_tmp[UTIL_PATH_SIZE + 32];

        /* use relative link */
        if (!path_relative(target, node, slink, sizeof(target)))
        	return -1;

        /* preserve link with correct target, do not replace node of other device */
        if (lstat(slink, &stats) == 0) {
                if (S_ISBLK(stats.st_mode) || S_ISCHR(stats.st_mode)) {
                        log_error("conflicting device node '%s' found, link to '%s' will not be created", slink, node);
                        return 0;
                }

                if (S_ISLNK(stats.st_mode)) {
                        char buf[UTIL_PATH_SIZE];
                        int len;

                        len = readlink(slink, buf, sizeof(buf));
                        if (len > 0 && len < (int)sizeof(buf)) {
                                buf[len] = '\0';
                                if (str_eq(target, buf)) {
                                        log_debug("preserve already existing symlink '%s' to '%s'", slink, target);
                                        label_fix(slink, true, false);
                                        utimensat(AT_FDCWD, slink, NULL, AT_SYMLINK_NOFOLLOW);
                                        return 0;
                                }
                        }
                }
        } else {
                log_debug("creating symlink '%s' to '%s'", slink, target);
                if (!symlink_create(target, slink))
                        return 0;
        }

        log_debug("atomically replace '%s'", slink);
        strscpyl(slink_tmp, sizeof(slink_tmp), slink, ".tmp-", devnum, NULL);
        unlink(slink_tmp);
        if (symlink_create(target, slink_tmp) != 0) {
                log_error("symlink '%s' '%s' failed: %m", target, slink_tmp);

                return -1;
        }

        if (rename(slink_tmp, slink) < 0) {
                log_error("rename '%s' '%s' failed: %m", slink_tmp, slink);
                unlink(slink_tmp);

                return -1;
        }

        return 0;
}

/* find device node of device with highest priority */
static const char *link_find_prioritized(struct udev_device *dev, bool add, const char *stackdir, char *buf, size_t bufsize)
{
        struct udev *udev = udev_device_get_udev(dev);
        DIR *dir;
        struct dirent *dent;
        int priority = 0;
        const char *target = NULL;

        if (add) {
                priority = udev_device_get_devlink_priority(dev);
                strscpy(buf, bufsize, udev_device_get_devnode(dev));
                target = buf;
        }

        dir = opendir(stackdir);
        if (dir == NULL)
                return target;
        for (;;) {
                struct udev_device *dev_db;

                dent = readdir(dir);
                if (dent == NULL || dent->d_name[0] == '\0')
                        break;
                if (dent->d_name[0] == '.')
                        continue;

                log_debug("found '%s' claiming '%s'", dent->d_name, stackdir);

                /* did we find ourself? */
                if (str_eq(dent->d_name, udev_device_get_id_filename(dev)))
                        continue;

                dev_db = udev_device_new_from_device_id(udev, dent->d_name);
                if (dev_db != NULL) {
                        const char *devnode;

                        devnode = udev_device_get_devnode(dev_db);
                        if (devnode != NULL) {
                                if (target == NULL || udev_device_get_devlink_priority(dev_db) > priority) {
                                        log_debug("'%s' claims priority %i for '%s'",
                                                  udev_device_get_syspath(dev_db), udev_device_get_devlink_priority(dev_db), stackdir);
                                        priority = udev_device_get_devlink_priority(dev_db);
                                        strscpy(buf, bufsize, devnode);
                                        target = buf;
                                }
                        }
                        udev_device_unref(dev_db);
                }
        }
        closedir(dir);
        return target;
}

/* manage "stack of names" with possibly specified device priorities */
static void link_update(struct udev_device *dev, const char *slink, bool add)
{
        char name_enc[UTIL_PATH_SIZE];
        char filename[UTIL_PATH_SIZE * 2];
        char dirname[UTIL_PATH_SIZE];
        const char *target;
        char buf[UTIL_PATH_SIZE];
        const char *devnum;

        devnum = udev_device_get_id_filename(dev);

        path_encode(name_enc, slink + strlen("/dev"), sizeof(name_enc));
        strscpyl(dirname, sizeof(dirname), UDEVRUNDIR "/links/", name_enc, NULL);
        strscpyl(filename, sizeof(filename), dirname, "/", devnum, NULL);

        if (!add && unlink(filename) == 0)
                rmdir(dirname);

        target = link_find_prioritized(dev, add, dirname, buf, sizeof(buf));
        if (target == NULL) {
                log_debug("no reference left, remove '%s'", slink);
                if (unlink(slink) == 0)
                        path_remove(slink);
        } else {
                log_debug("creating link '%s' to '%s'", slink, target);
                node_symlink(devnum, target, slink);
        }

        if (add) {
                int err;

                do {
                        int fd;

                        if (path_create(filename, 0755) < 0)
                                break;

                        fd = open(filename, O_WRONLY|O_CREAT|O_CLOEXEC|O_TRUNC|O_NOFOLLOW, 0444);
                        if (fd >= 0)
                                close(fd);
                        else
                                err = -errno;
                } while (err == -ENOENT);
        }
}

void udev_node_update_old_links(struct udev_device *dev, struct udev_device *dev_old)
{
        struct udev_list_entry *list_entry;

        /* update possible left-over symlinks */
        udev_list_entry_foreach(list_entry, udev_device_get_devlinks_list_entry(dev_old)) {
                const char *name = udev_list_entry_get_name(list_entry);
                struct udev_list_entry *list_entry_current;
                int found;

                /* check if old link name still belongs to this device */
                found = 0;
                udev_list_entry_foreach(list_entry_current, udev_device_get_devlinks_list_entry(dev)) {
                        const char *name_current = udev_list_entry_get_name(list_entry_current);

                        if (str_eq(name, name_current)) {
                                found = 1;
                                break;
                        }
                }
                if (found)
                        continue;

                log_debug("update old name, '%s' no longer belonging to '%s'",
                     name, udev_device_get_devpath(dev));
                link_update(dev, name, false);
        }
}

static int node_permissions_apply(struct udev_device *dev, bool apply,
                                  mode_t mode, uid_t uid, gid_t gid,
                                  struct udev_list *seclabel_list) {
        const char *devnode = udev_device_get_devnode(dev);
        dev_t devnum = udev_device_get_devnum(dev);
        struct stat stats;
        struct udev_list_entry *entry;

        if (str_eq(udev_device_get_subsystem(dev), "block"))
                mode |= S_IFBLK;
        else
                mode |= S_IFCHR;

        if (lstat(devnode, &stats) < 0) {
                log_debug("can not stat() node '%s' (%m)", devnode);
                return -1;
        }

        if (((stats.st_mode & S_IFMT) != (mode & S_IFMT)) || (stats.st_rdev != devnum)) {
                errno = EEXIST;
                log_debug("found node '%s' with non-matching devnum %s, skip handling",
                          udev_device_get_devnode(dev), udev_device_get_id_filename(dev));
                return -1;
        }

        if (apply) {
                bool selinux = false;

                if ((stats.st_mode & 0777) != (mode & 0777) || stats.st_uid != uid || stats.st_gid != gid) {
                        log_debug("set permissions %s, %#o, uid=%u, gid=%u", devnode, mode, uid, gid);
                        chmod(devnode, mode);
                        chown(devnode, uid, gid);
                } else {
                        log_debug("preserve permissions %s, %#o, uid=%u, gid=%u", devnode, mode, uid, gid);
                }

                /* apply SECLABEL{$module}=$label */
                udev_list_entry_foreach(entry, udev_list_get_entry(seclabel_list)) {
                        const char *name, *label;

                        name = udev_list_entry_get_name(entry);
                        label = udev_list_entry_get_value(entry);

                        if (str_eq(name, "selinux")) {
                                selinux = true;
                                if (label_apply(devnode, label) < 0)
                                        log_error("SECLABEL: failed to set SELinux label '%s': %m", label);
                                else
                                        log_debug("SECLABEL: set SELinux label '%s'", label);

                        } else
                                log_error("SECLABEL: unknown subsystem, ignoring '%s'='%s'", name, label);
                }

                /* set the defaults */
                if (!selinux)
                        label_fix(devnode, true, false);
        }

        /* always update timestamp when we re-use the node, like on media change events */
        utimensat(AT_FDCWD, devnode, NULL, 0);

        return 0;
}

void udev_node_add(struct udev_device *dev, bool apply,
                   mode_t mode, uid_t uid, gid_t gid,
                   struct udev_list *seclabel_list) {
        char filename[UTIL_PATH_SIZE];
        struct udev_list_entry *list_entry;
        const char *devnum;

        devnum = udev_device_get_id_filename(dev);

        log_debug("handling device node '%s', devnum=%s, mode=%#o, uid=%d, gid=%d",
                  udev_device_get_devnode(dev), devnum, mode, uid, gid);

        if (node_permissions_apply(dev, apply, mode, uid, gid, seclabel_list) < 0)
                return;

        /* always add /dev/{block,char}/$major:$minor */
        snprintf(filename, sizeof(filename), "/dev/%s/%u:%u",
                 str_eq(udev_device_get_subsystem(dev), "block") ? "block" : "char",
                 major(udev_device_get_devnum(dev)), minor(udev_device_get_devnum(dev)));
        node_symlink(devnum, udev_device_get_devnode(dev), filename);

        /* create/update symlinks, add symlinks to name index */
        udev_list_entry_foreach(list_entry, udev_device_get_devlinks_list_entry(dev))
                        link_update(dev, udev_list_entry_get_name(list_entry), true);
}

void udev_node_remove(struct udev_device *dev)
{
        struct udev_list_entry *list_entry;
        char filename[UTIL_PATH_SIZE];

        /* remove/update symlinks, remove symlinks from name index */
        udev_list_entry_foreach(list_entry, udev_device_get_devlinks_list_entry(dev))
                link_update(dev, udev_list_entry_get_name(list_entry), false);

        /* remove /dev/{block,char}/$major:$minor */
        snprintf(filename, sizeof(filename), "/dev/%s/%u:%u",
                 str_eq(udev_device_get_subsystem(dev), "block") ? "block" : "char",
                 major(udev_device_get_devnum(dev)), minor(udev_device_get_devnum(dev)));
        unlink(filename);
}
