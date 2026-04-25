#ifndef THREADS_SYNCH_H
#define THREADS_SYNCH_H

#include <list.h>
#include <stdbool.h>

/* 이 구간의 동작과 의도를 설명한다. */
struct semaphore {
	unsigned value;             /* 이 구간의 동작과 의도를 설명한다. */
	struct list waiters;        /* 이 구간의 동작과 의도를 설명한다. */
};

void sema_init (struct semaphore *, unsigned value);
void sema_down (struct semaphore *);
bool sema_try_down (struct semaphore *);
void sema_up (struct semaphore *);
void sema_self_test (void);

/* 이 구간의 동작과 의도를 설명한다. */
struct lock {
	struct thread *holder;      /* 이 구간의 동작과 의도를 설명한다. */
	struct semaphore semaphore; /* 이 구간의 동작과 의도를 설명한다. */
};

void lock_init (struct lock *);
void lock_acquire (struct lock *);
bool lock_try_acquire (struct lock *);
void lock_release (struct lock *);
bool lock_held_by_current_thread (const struct lock *);

/* 이 구간의 동작과 의도를 설명한다. */
struct condition {
	struct list waiters;        /* 이 구간의 동작과 의도를 설명한다. */
};

void cond_init (struct condition *);
void cond_wait (struct condition *, struct lock *);
void cond_signal (struct condition *, struct lock *);
void cond_broadcast (struct condition *, struct lock *);

/* 이 구간의 동작과 의도를 설명한다. */
#define barrier() asm volatile ("" : : : "memory")

#endif /* 이 구간의 동작과 의도를 설명한다. */
