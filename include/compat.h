#ifndef _COMPAT_H
#define _COMPAT_H

#ifdef __PCC__
/* pcc doesn't support many GCC attributes found in modern glibc headers.
   We define these BEFORE any system headers are included. */
#ifndef __attribute__
#define __attribute__(x)
#endif
#ifndef __cold__
#define __cold__
#endif
#ifndef __COLD
#define __COLD
#endif
#ifndef __leaf__
#define __leaf__
#endif
#ifndef __throw
#define __throw
#endif
#ifndef __THROW
#define __THROW
#endif
#ifndef __nonnull
#define __nonnull(x)
#endif
#ifndef __wur
#define __wur
#endif
#ifndef __warn_unused_result__
#define __warn_unused_result__
#endif
#ifndef MB_LEN_MAX
#define MB_LEN_MAX 16
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif

/* Standard Linux headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>

/* Fix implicit declarations */
char *gettxt(const char *msgid, const char *dflt_str);
int gmatch(const char *s, const char *p);
/* failed() is defined in defs.h */

/* Memory management */
/* These are defined in blok.c/stak.c but often implicitly declared */
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

/* Struct alignment/compatibility */
/* UnixWare's filehdr and fileblk must match for header casting to work */
/* We define them here to ensure consistency across translation units */

#define	BUFSIZE	128

struct fileblk
{
	int	fdes;
	unsigned flin;
	int	feof; /* Changed from BOOL to int to match usage */
	unsigned char	fsiz;
	unsigned char	*fnxt;
	unsigned char	*fend;
	unsigned char	**feval;
	struct fileblk	*fstak;
	unsigned char	fbuf[BUFSIZE];
};

/* filehdr matches fileblk initial members */
struct filehdr
{
	int	fdes;
	unsigned	flin;
	int 	feof;
	unsigned char	fsiz;
	unsigned char	*fnxt;
	unsigned char	*fend;
	unsigned char	**feval;
	struct fileblk	*fstak;
	unsigned char	_fbuf[1]; /* Dummy for alignment/access */
};

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY|O_WRONLY|O_RDWR)
#endif

#ifndef MAXLABEL
#define MAXLABEL 25
#endif

#endif /* _COMPAT_H */
