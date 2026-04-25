#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/interrupt.h"
#ifdef VM
#include "vm/vm.h"
#endif


/* 이 구간의 동작과 의도를 설명한다. */
enum thread_status {
	THREAD_RUNNING,     /* 이 구간의 동작과 의도를 설명한다. */
	THREAD_READY,       /* 이 구간의 동작과 의도를 설명한다. */
	THREAD_BLOCKED,     /* 이 구간의 동작과 의도를 설명한다. */
	THREAD_DYING        /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* 이 구간의 동작과 의도를 설명한다. */

/* 이 구간의 동작과 의도를 설명한다. */
#define PRI_MIN 0                       /* 이 구간의 동작과 의도를 설명한다. */
#define PRI_DEFAULT 31                  /* 이 구간의 동작과 의도를 설명한다. */
#define PRI_MAX 63                      /* 이 구간의 동작과 의도를 설명한다. */

/* 이 구간의 동작과 의도를 설명한다. */
/* 이 구간의 동작과 의도를 설명한다. */
struct thread {
	/* 이 구간의 동작과 의도를 설명한다. */
	tid_t tid;                          /* 이 구간의 동작과 의도를 설명한다. */
	enum thread_status status;          /* 이 구간의 동작과 의도를 설명한다. */
	char name[16];                      /* 이 구간의 동작과 의도를 설명한다. */
	int priority;                       /* 이 구간의 동작과 의도를 설명한다. */

	/* 이 구간의 동작과 의도를 설명한다. */
	int64_t wakeup_tick;

	/* 이 구간의 동작과 의도를 설명한다. */
	struct list_elem elem;              /* 이 구간의 동작과 의도를 설명한다. */

#ifdef USERPROG
	/* 이 구간의 동작과 의도를 설명한다. */
	uint64_t *pml4;                     /* 이 구간의 동작과 의도를 설명한다. */
#endif
#ifdef VM
	/* 이 구간의 동작과 의도를 설명한다. */
	struct supplemental_page_table spt;
#endif

	/* 이 구간의 동작과 의도를 설명한다. */
	struct intr_frame tf;               /* 이 구간의 동작과 의도를 설명한다. */
	unsigned magic;                     /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_sleep (int64_t wakeup_tick);
void thread_awake (int64_t current_tick);
void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

void do_iret (struct intr_frame *tf);

#endif /* 이 구간의 동작과 의도를 설명한다. */
