/*		copyright	"%c%" 	*/

#ident	"@(#)sh:common/cmd/sh/blok.c	1.9.14.1"
#ident "$Header$"
/*
 *	UNIX shell
 */

#include	"defs.h"
#include    <stdlib.h>
#include    <string.h>

/*
 *	storage allocator
 *	Replaced with standard malloc/free for Linux port
 */

unsigned	brkincr = BRKINCR;

#ifdef __STDC__
void *
#else
char *
#endif
alloc(nbytes)
	size_t nbytes;
{
    void *p = malloc(nbytes);
    if (!p) {
        error(0, nospace, nospaceid);
    }
    return p;
}

void
addblok(reqd)
	unsigned reqd;
{
    /* No-op in malloc based allocator */
}

#ifdef DEBUG
chkbptr(ptr)
	struct blk *ptr;
{
}

chkmem()
{
}
#endif
