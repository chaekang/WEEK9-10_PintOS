#ifndef THREADS_PALLOC_H
#define THREADS_PALLOC_H

#include <stdint.h>
#include <stddef.h>

/* 이 구간의 동작과 의도를 설명한다. */
enum palloc_flags {
	PAL_ASSERT = 001,           /* 이 구간의 동작과 의도를 설명한다. */
	PAL_ZERO = 002,             /* 이 구간의 동작과 의도를 설명한다. */
	PAL_USER = 004              /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
extern size_t user_page_limit;

uint64_t palloc_init (void);
void *palloc_get_page (enum palloc_flags);
void *palloc_get_multiple (enum palloc_flags, size_t page_cnt);
void palloc_free_page (void *);
void palloc_free_multiple (void *, size_t page_cnt);

#endif /* 이 구간의 동작과 의도를 설명한다. */
