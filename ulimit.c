/*		copyright	"%c%" 	*/

#ident	"@(#)sh:common/cmd/sh/ulimit.c	1.5.16.3"
#ident "$Header$"

/*
 * ulimit builtin
 */

#include <sys/resource.h>
#include <stdlib.h>

#ifndef RLIMIT_VMEM
#define RLIMIT_VMEM RLIMIT_AS
#endif
#include "defs.h"

extern	int	getrlimit();
extern	int	setrlimit();

static struct rlimtab {
	int     id;
	char	*name;
	char	*scale;
	int	divisor;
	ulong	mask;
} rlimtab[] = {
	{ RLIMIT_CPU,    "time",     "seconds",     1,    0x80000000 },
	{ RLIMIT_FSIZE,  "file",     "blocks",      512,  0xff800000 },
	{ RLIMIT_DATA,   "data",     "kbytes",      1024, 0xffc00000 },
	{ RLIMIT_STACK,  "stack",    "kbytes",      1024, 0xffc00000 },
	{ RLIMIT_CORE,   "coredump", "blocks",      512,  0xff800000 },
	{ RLIMIT_NOFILE, "nofiles",  "descriptors", 1,    0x80000000 },
	{ RLIMIT_AS,     "memory",   "kbytes",      1024, 0xffc00000 },
	{ -1, NULL, NULL, 0, 0 }
};

static struct rlimtab *
get_rlim_entry(int id)
{
	int i;
	for (i = 0; rlimtab[i].name != NULL; i++) {
		if (rlimtab[i].id == id)
			return &rlimtab[i];
	}
	return NULL;
}
void
sysulimit(argc, argv)
int argc;
char **argv;
{
	extern int opterr, optind;
	int savopterr, savoptind, savsp;
	char *savoptarg;
	char *args;
	int hard, soft, cnt, c, res;
	rlim_t limit;
	struct rlimit rlimit;
	char resources[RLIM_NLIMITS];

	for (res = 0;  res < RLIM_NLIMITS; res++)
		resources[res] = 0;

	savoptind = optind;
	savopterr = opterr;
	savsp = _sp;
	savoptarg = optarg;
	optind = 1;
	_sp = 1;
	opterr = 0;
	hard = 0;
	soft = 0;
	cnt = 0;

	while ((c = getopt(argc, argv, "HSacdfnstv")) != -1) {
		switch(c) {
			case 'S':
				soft++;
				continue;
			case 'H':
				hard++;
				continue;
			case 'a':
				for (res = 0;  res < RLIM_NLIMITS; res++)
					resources[res]++;
				cnt = RLIM_NLIMITS;
				continue;
			case 'c':
				res = RLIMIT_CORE;
				break;
			case 'd':	
				res = RLIMIT_DATA;
				break;
			case 'f':
				res = RLIMIT_FSIZE;
				break;
			case 'n':	
				res = RLIMIT_NOFILE;
				break;
			case 's':	
				res = RLIMIT_STACK;
				break;
			case 't':	
				res = RLIMIT_CPU;
				break;
			case 'v':	
				res = RLIMIT_VMEM;
				break;
			case '?':
				prusage(SYSULIMIT, ulimuse, ulimuseid);
				goto err;
		}
		resources[res]++;
		cnt++;
	}

	if (cnt == 0) {
		resources[res = RLIMIT_FSIZE]++;
	}

	/*
	 * if out of arguments, then print the specified resources
	 */

	if (optind == argc) {
		if (!hard && !soft)
			soft++;
		for (res = 0; res < RLIM_NLIMITS; res++) {
			struct rlimtab *entry;
			if (resources[res] == 0)
				continue;
			if ((entry = get_rlim_entry(res)) == NULL)
				continue;
			if (getrlimit(res, &rlimit) < 0)
				continue;
			if (cnt > 1) {
				prs_buff(entry->name);
				prc_buff('(');
				prs_buff(entry->scale);
				prc_buff(')');
				prc_buff(' ');
			}
			if (soft) {
				if (rlimit.rlim_cur == RLIM_INFINITY && cnt > 0)
					prs_buff("unlimited");
				else 
					prl_buff(rlimit.rlim_cur/entry->divisor);
			}
			if (hard && soft)
				prc_buff(':');
			if (hard) {
				if (rlimit.rlim_max == RLIM_INFINITY && cnt > 0)
					prs_buff("unlimited");
				else 
					prl_buff(rlimit.rlim_max/entry->divisor);
			}
			prc_buff('\n');
		}
		goto err;
	}

	if (cnt > 1 || optind + 1 != argc) {
		prusage(SYSULIMIT, ulimuse, ulimuseid);
		goto err;
	}

	if (eq(argv[optind],"unlimited"))
		limit = RLIM_INFINITY;
	else {
		struct rlimtab *entry = get_rlim_entry(res);
		if (entry == NULL) {
			error_fail(SYSULIMIT, badulimit, badulimitid);
			goto err;
		}
		args = argv[optind];

		limit = 0;
		do {
			if (*args < '0' || *args > '9') {
				error_fail(SYSULIMIT, badulimit, badulimitid);
				goto err;
			}
			limit = (limit * 10) + (*args - '0');
		} while (*++args);

		if (limit & entry->mask) {
			error_fail(SYSULIMIT, badulimit, badulimitid);
			goto err;
		}
 		limit *= entry->divisor;
	}

	if (getrlimit(res, &rlimit) < 0) {
		error_fail(SYSULIMIT, badulimit, badulimitid);
		goto err;
	}

	if (!hard && !soft)
		hard++, soft++;
	if (hard)
		rlimit.rlim_max = limit;
	if (soft)
		rlimit.rlim_cur = limit;

	if (setrlimit(res, &rlimit) < 0)
		error_fail(SYSULIMIT, badulimit, badulimitid);

err:
	optind = savoptind;
	opterr = savopterr;
	_sp = savsp;
	optarg = savoptarg;
}
