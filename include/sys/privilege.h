#ifndef _SYS_PRIVILEGE_H
#define _SYS_PRIVILEGE_H

typedef int priv_t;

/* Dummy definitions for privilege sets/types */
#define PS_TYPE 0
#define PS_WKG 0
#define PS_MAX 0
#define PS_FIX 0
#define PS_INH 0

/* Dummy privilege IDs */
#define P_OWNER 0
#define P_AUDIT 0
#define P_COMPAT 0
#define P_DACREAD 0
#define P_DACWRITE 0
#define P_DEV 0
#define P_FILESYS 0
#define P_MACREAD 0
#define P_MACWRITE 0
#define P_MOUNT 0
#define P_MULTIDIR 0
#define P_SETFLEVEL 0
#define P_SETPLEVEL 0
#define P_SETSPRIV 0
#define P_SETUID 0
#define P_SYSOPS 0
#define P_SETUPRIV 0
#define P_DRIVER 0
#define P_RTIME 0
#define P_MACUPGRADE 0
#define P_FSYSRANGE 0
#define P_AUDITWR 0
#define P_TSHAR 0
#define P_PLOCK 0
#define P_ALLPRIVS 0

#endif /* _SYS_PRIVILEGE_H */
