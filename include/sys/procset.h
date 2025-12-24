#ifndef _SYS_PROCSET_H
#define _SYS_PROCSET_H
#include <sys/types.h>
typedef int idtype_t;
typedef struct { idtype_t p_op; idtype_t p_lidtype; id_t p_lid; idtype_t p_ridtype; id_t p_rid; } procset_t;
#define P_PID 0
#define P_PPID 1
#define P_PGID 2
#define P_SID 3
#define P_ALL 7
#define POP_AND 0
#define POP_OR 1
#define POP_XOR 2
#define POP_DIFF 3
#endif
