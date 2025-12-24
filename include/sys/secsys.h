#ifndef _SYS_SECSYS_H
#define _SYS_SECSYS_H
#include <sys/types.h>
typedef int level_t;
#define ES_MACOPENLID   1
#define ES_MACADTLID    2
#define ES_MACPROCLID   3
#define ES_PRVINFO      4
#define ES_PRVID        ES_PRVINFO
#define ES_PRVSETCNT    5
#define ES_PRVSETS      6
extern int secsys(int, void *);
#endif
