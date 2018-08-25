/* liblxcapi
 *
 * Copyright © 2018 Christian Brauner <christian.brauner@ubuntu.com>.
 * Copyright © 2018 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __LXC_MACRO_H
#define __LXC_MACRO_H

#include "config.h"

#include <asm/types.h>
#include <linux/if_link.h>
#include <linux/loop.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <stdint.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/un.h>

/* Define __S_ISTYPE if missing from the C library. */
#ifndef __S_ISTYPE
#define __S_ISTYPE(mode, mask) (((mode)&S_IFMT) == (mask))
#endif

#if HAVE_LIBCAP
#ifndef CAP_SETFCAP
#define CAP_SETFCAP 31
#endif

#ifndef CAP_MAC_OVERRIDE
#define CAP_MAC_OVERRIDE 32
#endif

#ifndef CAP_MAC_ADMIN
#define CAP_MAC_ADMIN 33
#endif
#endif

#ifndef PR_CAPBSET_DROP
#define PR_CAPBSET_DROP 24
#endif

#ifndef CAP_SETUID
#define CAP_SETUID 7
#endif

#ifndef CAP_SETGID
#define CAP_SETGID 6
#endif

/* needed for cgroup automount checks, regardless of whether we
 * have included linux/capability.h or not */
#ifndef CAP_SYS_ADMIN
#define CAP_SYS_ADMIN 21
#endif

#ifndef HAVE_DECL_PR_CAPBSET_DROP
#define PR_CAPBSET_DROP 24
#endif

/* prctl */
#ifndef HAVE_DECL_PR_SET_NO_NEW_PRIVS
#define PR_SET_NO_NEW_PRIVS 38
#endif

#ifndef HAVE_DECL_PR_GET_NO_NEW_PRIVS
#define PR_GET_NO_NEW_PRIVS 39
#endif

#ifndef CGROUP_SUPER_MAGIC
#define CGROUP_SUPER_MAGIC 0x27e0eb
#endif

#ifndef CGROUP2_SUPER_MAGIC
#define CGROUP2_SUPER_MAGIC 0x63677270
#endif

#ifndef NSFS_MAGIC
#define NSFS_MAGIC 0x6e736673
#endif

/* We have two different magic values for overlayfs, yay. */
#ifndef OVERLAYFS_SUPER_MAGIC
#define OVERLAYFS_SUPER_MAGIC 0x794c764f
#endif

#ifndef OVERLAY_SUPER_MAGIC
#define OVERLAY_SUPER_MAGIC 0x794c7630
#endif

/* Calculate the number of chars needed to represent a given integer as a C
 * string. Include room for '-' to indicate negative numbers and the \0 byte.
 * This is based on systemd.
 */
#define INTTYPE_TO_STRLEN(type)                   \
	(2 + (sizeof(type) <= 1                   \
		  ? 3                             \
		  : sizeof(type) <= 2             \
			? 5                       \
			: sizeof(type) <= 4       \
			      ? 10                \
			      : sizeof(type) <= 8 \
				    ? 20          \
				    : sizeof(int[-2 * (sizeof(type) > 8)])))

/* Useful macros */
/* Maximum number for 64 bit integer is a string with 21 digits: 2^64 - 1 = 21 */
#define LXC_NUMSTRLEN64 INTTYPE_TO_STRLEN(int64_t)
#define LXC_LINELEN 4096
#define LXC_IDMAPLEN 4096
#define LXC_MAX_BUFFER 4096
/* /proc/       =    6
 *                +
 * <pid-as-str> =   LXC_NUMSTRLEN64
 *                +
 * /fd/         =    4
 *                +
 * <fd-as-str>  =   LXC_NUMSTRLEN64
 *                +
 * \0           =    1
 */
#define LXC_PROC_PID_FD_LEN (6 + LXC_NUMSTRLEN64 + 4 + LXC_NUMSTRLEN64 + 1)
/* /proc/pid-to-str/status\0 = (5 + 21 + 7 + 1) */
#define LXC_PROC_STATUS_LEN (5 + (LXC_NUMSTRLEN64) + 7 + 1)
#define LXC_CMD_DATA_MAX (MAXPATHLEN * 2)

/* loop devices */
#ifndef LO_FLAGS_AUTOCLEAR
#define LO_FLAGS_AUTOCLEAR 4
#endif

#ifndef LOOP_CTL_GET_FREE
#define LOOP_CTL_GET_FREE 0x4C82
#endif

/* memfd_create() */
#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif

#ifndef MFD_ALLOW_SEALING
#define MFD_ALLOW_SEALING 0x0002U
#endif

/**
 * BUILD_BUG_ON - break compile if a condition is true.
 * @condition: the condition which the compiler should know is false.
 *
 * If you have some code which relies on certain constants being equal, or
 * other compile-time-evaluated condition, you should use BUILD_BUG_ON to
 * detect if someone changes it.
 *
 * The implementation uses gcc's reluctance to create a negative array, but
 * gcc (as of 4.4) only emits that error for obvious cases (eg. not arguments
 * to inline functions).  So as a fallback we use the optimizer; if it can't
 * prove the condition is false, it will cause a link error on the undefined
 * "__build_bug_on_failed".  This error message can be harder to track down
 * though, hence the two different methods.
 */
#ifndef __OPTIMIZE__
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#else
extern int __build_bug_on_failed;
#define BUILD_BUG_ON(condition)					\
	do {							\
		((void)sizeof(char[1 - 2*!!(condition)]));	\
		if (condition) __build_bug_on_failed = 1;	\
	} while(0)
#endif

#define lxc_iterate_parts(__iterator, __splitme, __separators)                  \
	for (char *__p = NULL, *__it = strtok_r(__splitme, __separators, &__p); \
	     (__iterator = __it);                                               \
	     __iterator = __it = strtok_r(NULL, __separators, &__p))

#define prctl_arg(x) ((unsigned long)x)

/* networking */
#ifndef IFLA_LINKMODE
#define IFLA_LINKMODE 17
#endif

#ifndef IFLA_LINKINFO
#define IFLA_LINKINFO 18
#endif

#ifndef IFLA_NET_NS_PID
#define IFLA_NET_NS_PID 19
#endif

#ifndef IFLA_INFO_KIND
#define IFLA_INFO_KIND 1
#endif

#ifndef IFLA_VLAN_ID
#define IFLA_VLAN_ID 1
#endif

#ifndef IFLA_INFO_DATA
#define IFLA_INFO_DATA 2
#endif

#ifndef VETH_INFO_PEER
#define VETH_INFO_PEER 1
#endif

#ifndef IFLA_MACVLAN_MODE
#define IFLA_MACVLAN_MODE 1
#endif

#ifndef IFLA_NEW_NETNSID
#define IFLA_NEW_NETNSID 45
#endif

#ifndef IFLA_IF_NETNSID
#define IFLA_IF_NETNSID 46
#endif

#ifndef RTM_NEWNSID
#define RTM_NEWNSID 88
#endif

#ifndef NLMSG_ERROR
#define NLMSG_ERROR 0x2
#endif

#ifndef MACVLAN_MODE_PRIVATE
#define MACVLAN_MODE_PRIVATE 1
#endif

#ifndef MACVLAN_MODE_VEPA
#define MACVLAN_MODE_VEPA 2
#endif

#ifndef MACVLAN_MODE_BRIDGE
#define MACVLAN_MODE_BRIDGE 4
#endif

#ifndef MACVLAN_MODE_PASSTHRU
#define MACVLAN_MODE_PASSTHRU 8
#endif

/* Length of abstract unix domain socket socket address. */
#define LXC_AUDS_ADDR_LEN sizeof(((struct sockaddr_un *)0)->sun_path)

/* mount */
#ifndef MS_REC
#define MS_REC 16384
#endif

#ifndef MS_SLAVE
#define MS_SLAVE (1 << 19)
#endif

/* open */
#ifndef O_PATH
#define O_PATH      010000000
#endif

#ifndef O_NOFOLLOW
#define O_NOFOLLOW  00400000
#endif

/* sockets */
#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC 02000000
#endif

/* pointer conversion macros */
#define PTR_TO_INT(p) ((int)((intptr_t)(p)))
#define INT_TO_PTR(u) ((void *)((intptr_t)(u)))

#endif /* __LXC_MACRO_H */
