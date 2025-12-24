#ifndef _PFMT_H
#define _PFMT_H
#include <stdio.h>
#include <stdarg.h>
#define MM_STD 0
#define MM_NOSTD 0x100
#define MM_GET 0
#define MM_NOGET 0x200
#define MM_ACTION 0x400
#define MM_NOCONSOLE 0
#define MM_CONSOLE 0x800
#define MM_NULLMC 0
#define MM_HARD 0x1000
#define MM_SOFT 0x2000
#define MM_FIRM 0x4000
#define MM_APPL 0x8000
#define MM_UTIL 0x10000
#define MM_OPSYS 0x20000
#define MM_SVCMD (MM_UTIL|MM_SOFT)
#define MM_ERROR 0
#define MM_HALT 1
#define MM_WARNING 2
#define MM_INFO 3
extern int pfmt(FILE *, long, const char *, ...);
extern int lfmt(FILE *, long, const char *, ...);
extern int vpfmt(FILE *, long, const char *, va_list);
extern int vlfmt(FILE *, long, const char *, va_list);
extern char *setcat(const char *);
extern int setlabel(const char *);
extern int addsev(int, const char *);
extern char *gettxt(const char *, const char *);
#endif
#define MAXLABEL 25
