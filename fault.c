/*		copyright	"%c%" 	*/

#ident	"@(#)sh:common/cmd/sh/fault.c	1.13.29.8"
#ident  "$Header$"
/*
 * UNIX shell
 */
#include	<pfmt.h>
#include	<unistd.h>
#include	<signal.h>
#include	"defs.h"
#include	<sys/procset.h>

void done();
void stdsigs();
void oldsigs();
void chktrap();

extern int str2sig();

static void fault();
static BOOL sleeping = 0;
static unsigned char *trapcom[MAXTRAP];
static BOOL trapflg[MAXTRAP] =
{
	0,
	0,	/* hangup */
	0,	/* interrupt */
	0,	/* quit */
	0,	/* illegal instr */
	0,	/* trace trap */
	0,	/* IOT */
	0,	/* EMT */
	0,	/* float pt. exp */
	0,	/* kill */
	0, 	/* bus error */
	0,	/* memory faults */
	0,	/* bad sys call */
	0,	/* bad pipe call */
	0,	/* alarm */
	0, 	/* software termination */
	0,	/* unassigned */
	0,	/* unassigned */
	0,	/* death of child */
	0,	/* power fail */
	0,	/* window size change */
	0,	/* urgent IO condition */
	0,	/* pollable event occured */
	0,	/* stopped by signal */
	0,	/* stopped by user */
	0,	/* continued */
	0,	/* stopped by tty input */
	0,	/* stopped by tty output */
	0,	/* virtual timer expired */
	0,	/* profiling timer expired */
	0,	/* exceeded cpu limit */
	0,	/* exceeded file size limit */
};

static void (*(sigval[MAXTRAP]))();
static int sigval_initialized = 0;

/*
 * Initialize signal handler table using symbolic names for portability.
 * Signal numbers vary across OSes (Linux, FreeBSD, macOS, etc.)
 */
static void
init_sigval()
{
	int i;
	
	if (sigval_initialized) return;
	
	/* Default all signals to 0 (SIG_DFL) */
	for (i = 0; i < MAXTRAP; i++)
		sigval[i] = 0;
	
	/* Fatal signals - shell exits */
#ifdef SIGHUP
	sigval[SIGHUP] = done;
#endif
#ifdef SIGILL
	sigval[SIGILL] = done;
#endif
#ifdef SIGTRAP
	sigval[SIGTRAP] = done;
#endif
#ifdef SIGABRT
	sigval[SIGABRT] = done;
#endif
#ifdef SIGBUS
	sigval[SIGBUS] = done;
#endif
#ifdef SIGFPE
	sigval[SIGFPE] = done;
#endif
#ifdef SIGUSR1
	sigval[SIGUSR1] = done;
#endif
#ifdef SIGUSR2
	sigval[SIGUSR2] = done;
#endif
#ifdef SIGPIPE
	sigval[SIGPIPE] = done;
#endif
#ifdef SIGALRM
	sigval[SIGALRM] = done;
#endif
#ifdef SIGXCPU
	sigval[SIGXCPU] = done;
#endif
#ifdef SIGXFSZ
	sigval[SIGXFSZ] = done;
#endif
#ifdef SIGVTALRM
	sigval[SIGVTALRM] = done;
#endif
#ifdef SIGPROF
	sigval[SIGPROF] = done;
#endif
#ifdef SIGIO
	sigval[SIGIO] = done;
#endif
#ifdef SIGPWR
	sigval[SIGPWR] = done;
#endif
#ifdef SIGSYS
	sigval[SIGSYS] = done;
#endif
#ifdef SIGSTKFLT
	sigval[SIGSTKFLT] = done;
#endif

	/* Interactive signals - shell handles but continues */
#ifdef SIGINT
	sigval[SIGINT] = fault;
#endif
#ifdef SIGQUIT
	sigval[SIGQUIT] = fault;
#endif
	/* SIGSEGV: Let default handler crash cleanly on 64-bit Linux.
	 * The legacy sbrk stack expansion trick doesn't work. */
#ifdef SIGTERM
	sigval[SIGTERM] = fault;
#endif


	/* Job control signals - leave as default (0) */
	/* SIGCHLD, SIGCONT, SIGSTOP, SIGTSTP, SIGTTIN, SIGTTOU, SIGWINCH */
	/* These are handled via waitpid() or terminal driver */

	sigval_initialized = 1;
}

static int
ignoring(i)
register int i;
{
	struct sigaction act;
	if (trapflg[i] & SIGIGN)
		return 1;
	/*
	** O isn't a real signal
	*/
	if(i == 0)	{
		return 0;
	}
	(void)sigaction(i, 0, &act);
	if (act.sa_handler == SIG_IGN) {
		trapflg[i] |= SIGIGN;
		return 1;
	}
	return 0;
}

static void
clrsig(i)
int	i;
{
	if (trapcom[i] != 0) {
		free(trapcom[i]);
		trapcom[i] = 0;
	}
	if (trapflg[i] & SIGMOD) {
		trapflg[i] &= ~SIGMOD;
		handle(i, sigval[i]);
	}
}

void
done(int sig)
{
	register unsigned char	*t = trapcom[0];

	if (t)
	{
		trapcom[0] = 0;
		execexp(t, 0);
		free(t);
	}
	else
		chktrap();

	rmtemp(0);
	rmfunctmp();

#ifdef ACCT
	doacct();
#endif
	(void)endjobs(0);
	clearjobs();
	if (sig) {
		sigset_t set;
		(void)sigemptyset(&set);
		(void)sigaddset(&set, sig);
		(void)sigprocmask(SIG_UNBLOCK, &set, 0);
		handle(sig, SIG_DFL);
		kill(mypid, sig);
	}
	exit(exitval);
}

static void 
fault(sig)
register int	sig;
{
	register int flag;
	
	switch (sig) {
		case SIGSEGV:
			/* On 64-bit Linux, the legacy sbrk-based stack expansion
			 * does not work and causes an infinite loop. Make it fatal.
			 */
			write(2, "Segmentation fault\n", 19);
			_exit(139);  /* 128 + SIGSEGV(11) */
		case SIGALRM:
			if (sleeping)
				return;
			break;
	}


	if (trapcom[sig])
		flag = TRAPSET;
	else
		flag = SIGSET;
	/* ignore SIGTERM, SIGQUIT, and SIGINT when "-i" option is used. */
	if ( (flags&intflg)!=intflg || (sig!=SIGTERM && sig!=SIGQUIT && sig!=SIGINT))
		trapnote |= flag;
	trapflg[sig] |= flag;
}

int
handle(sig, func)
	int sig; 
	void (*func)();
{
	struct sigaction act, oact;
	if (func == SIG_IGN && (trapflg[sig] & SIGIGN))
		return 0;
	(void)sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = func;
	(void)sigaction(sig, &act, &oact);
	if (func == SIG_IGN)
		trapflg[sig] |= SIGIGN;
	else
		trapflg[sig] &= ~SIGIGN;
	return (func != oact.sa_handler);
}

/*
 * Called from main() to set up standard signal dispositions.
 *
 * Most ignored or masked signals remain ignored or masked.
 * SIGSEGV is left to the default handler on 64-bit Linux since
 * the legacy sbrk-based stack expansion no longer works.
 */

void
stdsigs()
{
	register int	i;

	init_sigval();  /* Initialize signal table with symbolic names */

	for (i = 1; i < MAXTRAP; i++) {
		if (sigval[i] == 0)
			continue;
		if (ignoring(i))
			continue;
		handle(i, sigval[i]);
	}
}


void
oldsigs()
{
	register int	i;
	register unsigned char	*t;

	i = MAXTRAP;
	while (i--)
	{
		t = trapcom[i];
		if (t == 0 || *t)
			clrsig(i);
		trapflg[i] = 0;
	}
	trapnote = 0;
}

/*
 * check for traps
 */

void
chktrap()
{
	register int	i = MAXTRAP;
	register unsigned char	*t;

	trapnote &= ~TRAPSET;
	while (--i)
	{
		if (trapflg[i] & TRAPSET)
		{
			trapflg[i] &= ~TRAPSET;
			t = trapcom[i];
			if (t)
			{
				int	savxit = exitval;
				execexp(t, 0);
				exitval = savxit;
				exitset();
			}
		}
	}
}

void
systrap(int argc, unsigned char *argv[])
{
	int sig;

	if (argc == 1) {
		for (sig = 0; sig < MAXTRAP; sig++) {
			if (trapcom[sig]) {
				prn_buff(sig);
				prs_buff(colon);
				prs_buff(trapcom[sig]);
				prc_buff(NL);
			}
		}
	} else {
		char *a1 = *(argv+1);
		char *cka1 = a1;
		BOOL noa1;

		while (digit(*cka1))
			cka1++;
		noa1=((a1!=cka1&&(*cka1==0)))||((a1==cka1)&&(str2sig(a1,&sig)==0));
		if (noa1 == 0)
			++argv;
		(void) str2sig(a1,&sig);
		while (*++argv) {
			if (str2sig(*argv,&sig) < 0 ||
			  sig >= MAXTRAP || sig < MINTRAP || 
			  sig == SIGSEGV || sig == SIGCHLD)
				error_fail(SYSTRAP, badtrap, badtrapid);
			else if (sig == SIGALRM)
				warning(SYSTRAP, badtrap, badtrapid);
			else if (noa1)
				clrsig(sig);
                        else if (*a1) {
				if (trapflg[sig] & SIGMOD || !ignoring(sig)) {
					handle(sig, fault);
					trapflg[sig] |= SIGMOD;
					replace(&trapcom[sig], a1, 0);
				}
			} else if (handle(sig, SIG_IGN)) {
				trapflg[sig] |= SIGMOD;
				replace(&trapcom[sig], a1, 0);
			}
		}
	}
}

void
mysleep(ticks)
int ticks;
{
	sigset_t set, oset;
	struct sigaction act, oact;

	/*
	 * add SIGALRM to mask
	 */

	(void)sigemptyset(&set);
	(void)sigaddset(&set, SIGALRM);
	(void)sigprocmask(SIG_BLOCK, &set, &oset);

	/*
	 * catch SIGALRM
	 */

	(void)sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = fault;
	(void)sigaction(SIGALRM, &act, &oact);

	/*
	 * start alarm and wait for signal
	 */

	set = oset;
	(void)sigdelset(&set, SIGALRM);
	(void)alarm(ticks);
	sleeping = 1;
	(void)sigsuspend(&set);
	sleeping = 0;

	/*
	 * reset alarm, catcher and mask
	 */

	(void)alarm(0); 
	(void)sigaction(SIGALRM, &oact, NULL);
	(void)sigprocmask(SIG_SETMASK, &oset, 0);
}
