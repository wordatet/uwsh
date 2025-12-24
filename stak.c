/*		copyright	"%c%" 	*/

#ident	"@(#)sh:common/cmd/sh/stak.c	1.8.7.2"
#ident "$Header$"
/*
 * UNIX shell
 */

#include	"defs.h"
#include    <stdlib.h>

/* ========	storage allocation	======== */

/* 
 * For Linux port: Use a large fixed buffer for the stack 
 * instead of growing via sbrk.
 */
#define STAK_SIZE (8 * 1024 * 1024) /* 8MB stack */
static unsigned char *stak_buffer = NULL;

void
initstak()
{
    if (!stak_buffer) {
        stak_buffer = malloc(STAK_SIZE);
        if (!stak_buffer) {
             error(0, nospace, nospaceid);
        }
        stakbas = stakbot = staktop = stak_buffer;
        brkend = stak_buffer + STAK_SIZE;
    }
}

#include <stdio.h>

#ifdef __STDC__
void *
#else
unsigned char *
#endif
getstak(asize)			/* allocate requested stack */
int	asize;
{
	register unsigned char	*oldstak;
	register int	size;

    if (!stak_buffer) initstak();

	size = round(asize, BYTESPERWORD);
	oldstak = stakbot;
	staktop = stakbot += size;
    


    if (staktop >= brkend) {
        error(0, nospace, nospaceid);
    }
    
	return(oldstak);
}

/*
 * set up stack for local use
 * should be followed by `endstak'
 */
unsigned char *
locstak()
{
    if (!stak_buffer) initstak();

	if (brkend - stakbot < BRKINCR)
	{
        /* In fixed mode, we can't really grow beyond STAK_SIZE */
		error(0, nospace, nospaceid);
	}
	return(stakbot);
}

unsigned char *
savstak()
{
	assert(staktop == stakbot);
	return(stakbot);
}

unsigned char *
endstak(argp)		/* tidy up after `locstak' */
register unsigned char	*argp;
{
	register unsigned char	*oldstak;

	*argp++ = 0;
	oldstak = stakbot;
	stakbot = staktop = (unsigned char *)round(argp, BYTESPERWORD);
	return(oldstak);
}

void
tdystak(x)		/* try to bring stack back to x */
register unsigned char	*x;
{
	while ((unsigned char *)stakbsy > x)
	{
		free(stakbsy);
		stakbsy = stakbsy->word;
	}
	staktop = stakbot = max(x, stakbas);
	rmtemp(x);
}

void
stakchk()
{
    /* No-op in fixed stack mode */
	/* if ((brkend - stakbas) > BRKINCR + BRKINCR)
		(void)setbrk(-BRKINCR); */
}

#ifdef __STDC__
void *
#else
unsigned char *
#endif
cpystak(x)
unsigned char	*x;
{
	return(endstak(movstr(x, locstak())));
}
