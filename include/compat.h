#ifndef _COMPAT_H
#define _COMPAT_H

/* Standard Linux headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

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
