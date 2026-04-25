#ifndef THREADS_VADDR_H
#define THREADS_VADDR_H

#include <debug.h>
#include <stdint.h>
#include <stdbool.h>

#include "threads/loader.h"

/* 이 구간의 동작과 의도를 설명한다. */

#define BITMASK(SHIFT, CNT) (((1ul << (CNT)) - 1) << (SHIFT))

/* 이 구간의 동작과 의도를 설명한다. */
#define PGSHIFT 0                          /* 이 구간의 동작과 의도를 설명한다. */
#define PGBITS  12                         /* 이 구간의 동작과 의도를 설명한다. */
#define PGSIZE  (1 << PGBITS)              /* 이 구간의 동작과 의도를 설명한다. */
#define PGMASK  BITMASK(PGSHIFT, PGBITS)   /* 이 구간의 동작과 의도를 설명한다. */

/* 이 구간의 동작과 의도를 설명한다. */
#define pg_ofs(va) ((uint64_t) (va) & PGMASK)

#define pg_no(va) ((uint64_t) (va) >> PGBITS)

/* 이 구간의 동작과 의도를 설명한다. */
#define pg_round_up(va) ((void *) (((uint64_t) (va) + PGSIZE - 1) & ~PGMASK))

/* 이 구간의 동작과 의도를 설명한다. */
#define pg_round_down(va) (void *) ((uint64_t) (va) & ~PGMASK)

/* 이 구간의 동작과 의도를 설명한다. */
#define KERN_BASE LOADER_KERN_BASE

/* 이 구간의 동작과 의도를 설명한다. */
#define USER_STACK 0x47480000

/* 이 구간의 동작과 의도를 설명한다. */
#define is_user_vaddr(vaddr) (!is_kernel_vaddr((vaddr)))

/* 이 구간의 동작과 의도를 설명한다. */
#define is_kernel_vaddr(vaddr) ((uint64_t)(vaddr) >= KERN_BASE)

// 한국어 주석
/* 이 구간의 동작과 의도를 설명한다. */
#define ptov(paddr) ((void *) (((uint64_t) paddr) + KERN_BASE))

/* 이 구간의 동작과 의도를 설명한다. */
#define vtop(vaddr) \
({ \
	ASSERT(is_kernel_vaddr(vaddr)); \
	((uint64_t) (vaddr) - (uint64_t) KERN_BASE);\
})

#endif /* 이 구간의 동작과 의도를 설명한다. */
