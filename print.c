/*		copyright	"%c%" 	*/

#ident	"@(#)sh:common/cmd/sh/print.c	1.12.12.3"
#ident "$Header$"
/*
 * UNIX shell
 *
 */

#include	"defs.h"
#include	<sys/param.h>
#include	<pfmt.h>

#define		BUFLEN		256

unsigned char numbuf[12];

static unsigned char buffer[BUFLEN];
static unsigned char *bufp = buffer;
static int idx = 0;
static int buffd = 1;

void	prc_buff();
void	prs_buff();
void	prn_buff();
void	prs_cntl();
void	prn_buff();
void	prs_cntl();
void	prp();
void	prs();
void	prc();
void	prt();
void	prn();
void	itos();
void	flushb();

static unsigned char *octal();

/*
 * printing and io conversion
 */
void
prp()
{
	if ((flags & prompt) == 0 && cmdadr)
	{
		prs_cntl(cmdadr);
		prs(gettxt(colonid, colon));
	}
}

void
prs(s)
unsigned char	*s;
{
	if (s)
		(void)write(output, s, length(s) - 1);
}

void
prc(c)
unsigned char	c;
{
	if (c)
		(void)write(output, &c, 1);
}

void
prt(t)
long	t;
{
	register int hr, min, sec;

	t += HZ / 2;
	t /= HZ;
	sec = t % 60;
	t /= 60;
	min = t % 60;

	hr = t / 60;
	if (hr)
	{
		prn_buff(hr);
		prs_buff("h");
	}

	prn_buff(min);
	prs_buff("m");
	prn_buff(sec);
	prs_buff("s");
}

void
prn(n)
	int	n;
{
	itos(n);

	prs(numbuf);
}

void
itos(n)
{
	register unsigned char *abuf;
	register unsigned a, i;
	int pr, d;

	abuf = numbuf;

	pr = FALSE;
	a = n;
	for (i = 10000; i != 1; i /= 10)
	{
		if ((pr |= (d = a / i)))
			*abuf++ = d + '0';
		a %= i;
	}
	*abuf++ = a + '0';
	*abuf++ = 0;
}

int
stoi(icp)
unsigned char	*icp;
{
	register unsigned char	*cp = icp;
	register int	r = 0;
	register unsigned char	c;

	if(icp)
	while ((c = *cp, digit(c)) && c && r >= 0)
	{
		r = r * 10 + c - '0';
		cp++;
	}
	if (r < 0 || cp == icp)
		failed(0, icp, badnum, badnumid); /*does not return*/
	return(r);
}

int
ltos(n)
long n;
{
	int i;

	numbuf[11] = '\0';
	for (i = 10; i >= 0; i--) {
		numbuf[i] = n % 10 + '0';
		if ((n /= 10) == 0)
			break;
	}
	return i;
}

void
flushb()
{
	if (idx)
	{
		bufp[idx] = '\0';
		(void)write(buffd, bufp, length(bufp) - 1);
		idx = 0;
	}
}

void
prc_buff(c)
	unsigned char c;
{
	if (c)
	{
		if (buffd != -1 && idx + 1 >= BUFLEN)
			flushb();

		bufp[idx++] = c;
	}
	else
	{
		flushb();
		(void)write(buffd, &c, 1);
	}
}

void
prs_buff(s)
	unsigned char *s;
{
	register int len = length(s) - 1;

	if (buffd != -1 && idx + len >= BUFLEN)
		flushb();

	if (buffd != -1 && len >= BUFLEN)
		(void)write(buffd, s, len);
	else
	{
		movstr(s, &bufp[idx]);
		idx += len;
	}
}

static unsigned char *
octal(c, ptr)
unsigned char c;
unsigned char *ptr;
{
	*ptr++ = '\\';
	*ptr++ = ((unsigned int)c >> 6) + '0';
	*ptr++ = (((unsigned int)c >> 3) & 07) + '0';
	*ptr++ = (c & 07) + '0';
	return(ptr);
}

void
prs_cntl(s)
unsigned char *s;
{
	register int n;
	wchar_t l;
	unsigned char *olds = s;
	register unsigned char *ptr = bufp;
	register wchar_t c;
	n = mbtowc(&l, (const char *)s, MULTI_BYTE_MAX);
	while(n != 0)
	{
		if(n < 0)
			ptr = octal(*s++, ptr);
		else { 
			c = l;
			s += n;
			if(!wisprint(c))
			{
				if(c < '\040' && c > 0)
				{
				/*
			 	 * assumes ASCII char
			 	 * translate a control character 
			 	 * into a printable sequence 
			 	 */
					*ptr++ = '^';
					*ptr++ = (c + 0100);	
				}
				else if (c == 0177) 
				{	/* '\0177' does not work */
					*ptr++ = '^';
					*ptr++ = '?';
				}
				else    /* unprintable 8-bit byte sequence 
				 	 * assumes all legal multibyte
				 	 * sequences are 
				 	 * printable 
				 	 */
					ptr = octal(*olds, ptr);
			}
			else
				while(n--)
					*ptr++ = *olds++;
		}
		if(buffd != -1 && ptr >= &bufp[BUFLEN-4]) {
			*ptr='\0';
			prs(bufp);
			ptr = bufp;
		}
		olds = s;
		n = mbtowc(&l, (const char *)s, MULTI_BYTE_MAX);
	}
	*ptr = '\0';
	prs(bufp);
}

void
prl_buff(l)
long	l;
{
	prs_buff(&numbuf[ltos(l)]);
}

void
prn_buff(n)
int	n;
{
	itos(n);

	prs_buff(numbuf);
}

int
setb(fd)
int fd;
{
	int ofd;

	if ((ofd = buffd) == -1) {
		if (bufp[idx-1])
			bufp[idx++] = 0;
		(void)endstak(bufp+idx);
	} else
		flushb();
	if ((buffd = fd) == -1)
		bufp = locstak();
	else
		bufp = buffer;
	idx = 0;
	return ofd;
}

