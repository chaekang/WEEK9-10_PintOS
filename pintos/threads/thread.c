#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "intrinsic.h"
#ifdef USERPROG
#include "userprog/process.h"
#endif

/* 이 구간의 동작과 의도를 설명한다. */
#define THREAD_MAGIC 0xcd6abf4b

/* 이 구간의 동작과 의도를 설명한다. */
#define THREAD_BASIC 0xd42df210

/* 이 구간의 동작과 의도를 설명한다. */
static struct list ready_list;

/* 자고 있는 스레드들의 리스트 */
static struct list sleep_list;

/* 이 구간의 동작과 의도를 설명한다. */
static struct thread *idle_thread;

/* 이 구간의 동작과 의도를 설명한다. */
static struct thread *initial_thread;

/* 이 구간의 동작과 의도를 설명한다. */
static struct lock tid_lock;

/* 이 구간의 동작과 의도를 설명한다. */
static struct list destruction_req;

/* 이 구간의 동작과 의도를 설명한다. */
static long long idle_ticks;    /* 이 구간의 동작과 의도를 설명한다. */
static long long kernel_ticks;  /* 이 구간의 동작과 의도를 설명한다. */
static long long user_ticks;    /* 이 구간의 동작과 의도를 설명한다. */

/* 이 구간의 동작과 의도를 설명한다. */
#define TIME_SLICE 4            /* 이 구간의 동작과 의도를 설명한다. */
static unsigned thread_ticks;   /* 이 구간의 동작과 의도를 설명한다. */

/* 이 구간의 동작과 의도를 설명한다. */
bool thread_mlfqs;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);
static struct thread *next_thread_to_run (void);
static void init_thread (struct thread *, const char *name, int priority);
static void do_schedule(int status);
static bool is_wakeup_tick_earlier (const struct list_elem *, const struct list_elem *, void *aux);
static void schedule (void);

static tid_t allocate_tid (void);

/* 이 구간의 동작과 의도를 설명한다. */
#define is_thread(t) ((t) != NULL && (t)->magic == THREAD_MAGIC)

/* 이 구간의 동작과 의도를 설명한다. */
#define running_thread() ((struct thread *) (pg_round_down (rrsp ())))


// 한국어 주석
// 한국어 주석
// 한국어 주석
static uint64_t gdt[3] = { 0, 0x00af9a000000ffff, 0x00cf92000000ffff };

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_init (void) {
	ASSERT (intr_get_level () == INTR_OFF);

	/* 이 구간의 동작과 의도를 설명한다. */
	struct desc_ptr gdt_ds = {
		.size = sizeof (gdt) - 1,
		.address = (uint64_t) gdt
	};
	lgdt (&gdt_ds);

	/* 이 구간의 동작과 의도를 설명한다. */
	lock_init (&tid_lock);
	list_init (&ready_list);
	list_init (&sleep_list);
	list_init (&destruction_req);

	/* 이 구간의 동작과 의도를 설명한다. */
	initial_thread = running_thread ();
	init_thread (initial_thread, "main", PRI_DEFAULT);
	initial_thread->status = THREAD_RUNNING;
	initial_thread->tid = allocate_tid ();
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_start (void) {
	/* 이 구간의 동작과 의도를 설명한다. */
	struct semaphore idle_started;
	sema_init (&idle_started, 0);
	thread_create ("idle", PRI_MIN, idle, &idle_started);

	/* 이 구간의 동작과 의도를 설명한다. */
	intr_enable ();

	/* 이 구간의 동작과 의도를 설명한다. */
	sema_down (&idle_started);
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_tick (void) {
	struct thread *t = thread_current ();

	/* 이 구간의 동작과 의도를 설명한다. */
	if (t == idle_thread)
		idle_ticks++;
#ifdef USERPROG
	else if (t->pml4 != NULL)
		user_ticks++;
#endif
	else
		kernel_ticks++;

	/* 이 구간의 동작과 의도를 설명한다. */
	if (++thread_ticks >= TIME_SLICE)
		intr_yield_on_return ();
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_print_stats (void) {
	printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
			idle_ticks, kernel_ticks, user_ticks);
}

/* 이 구간의 동작과 의도를 설명한다. */
tid_t
thread_create (const char *name, int priority,
		thread_func *function, void *aux) {
	struct thread *t;
	tid_t tid;

	ASSERT (function != NULL);

	/* 이 구간의 동작과 의도를 설명한다. */
	t = palloc_get_page (PAL_ZERO);
	if (t == NULL)
		return TID_ERROR;

	/* 이 구간의 동작과 의도를 설명한다. */
	init_thread (t, name, priority);
	tid = t->tid = allocate_tid ();

	/* 이 구간의 동작과 의도를 설명한다. */
	t->tf.rip = (uintptr_t) kernel_thread;
	t->tf.R.rdi = (uint64_t) function;
	t->tf.R.rsi = (uint64_t) aux;
	t->tf.ds = SEL_KDSEG;
	t->tf.es = SEL_KDSEG;
	t->tf.ss = SEL_KDSEG;
	t->tf.cs = SEL_KCSEG;
	t->tf.eflags = FLAG_IF;

	/* 이 구간의 동작과 의도를 설명한다. */
	thread_unblock (t);

	return tid;
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_awake (int64_t current_tick) {
	while (!list_empty (&sleep_list)) {
		struct list_elem *front = list_front (&sleep_list);
		struct thread *sleeping_thread = list_entry (front, struct thread, elem);
		if (sleeping_thread->wakeup_tick > current_tick) {
			break;
		}

		list_pop_front (&sleep_list);
		thread_unblock (sleeping_thread);
	}
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_sleep (int64_t wakeup_tick) {
	struct thread *current = thread_current ();
	enum intr_level old_level;

	old_level = intr_disable ();
	current->wakeup_tick = wakeup_tick;
	list_insert_ordered (&sleep_list, &current->elem, is_wakeup_tick_earlier, NULL);

	thread_block ();

	intr_set_level (old_level);
}


/* 이 구간의 동작과 의도를 설명한다. */
void
thread_block (void) {
	ASSERT (!intr_context ());
	ASSERT (intr_get_level () == INTR_OFF);
	thread_current ()->status = THREAD_BLOCKED;
	schedule ();
}

/* 이 구간의 동작과 의도를 설명한다. */
static bool
is_wakeup_tick_earlier (const struct list_elem *a, const struct list_elem *b, void *aux UNUSED) {
	struct thread *thread_a = list_entry (a, struct thread, elem);
	struct thread *thread_b = list_entry (b, struct thread, elem);
	if (thread_a->wakeup_tick != thread_b->wakeup_tick) {
		return thread_a->wakeup_tick < thread_b->wakeup_tick;
	}
	return false;
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_unblock (struct thread *t) {
	enum intr_level old_level;

	ASSERT (is_thread (t));

	old_level = intr_disable ();
	ASSERT (t->status == THREAD_BLOCKED);
	list_push_back (&ready_list, &t->elem);
	t->status = THREAD_READY;
	intr_set_level (old_level);
}

/* 이 구간의 동작과 의도를 설명한다. */
const char *
thread_name (void) {
	return thread_current ()->name;
}

/* 이 구간의 동작과 의도를 설명한다. */
struct thread *
thread_current (void) {
	struct thread *t = running_thread ();

	/* 이 구간의 동작과 의도를 설명한다. */
	ASSERT (is_thread (t));
	ASSERT (t->status == THREAD_RUNNING);

	return t;
}

/* 이 구간의 동작과 의도를 설명한다. */
tid_t
thread_tid (void) {
	return thread_current ()->tid;
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_exit (void) {
	ASSERT (!intr_context ());

#ifdef USERPROG
	process_exit ();
#endif

	/* 이 구간의 동작과 의도를 설명한다. */
	intr_disable ();
	do_schedule (THREAD_DYING);
	NOT_REACHED ();
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_yield (void) {
	struct thread *curr = thread_current ();
	enum intr_level old_level;

	ASSERT (!intr_context ());

	old_level = intr_disable ();
	if (curr != idle_thread)
		list_push_back (&ready_list, &curr->elem);
	do_schedule (THREAD_READY);
	intr_set_level (old_level);
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_set_priority (int new_priority) {
	thread_current ()->priority = new_priority;
}

/* 이 구간의 동작과 의도를 설명한다. */
int
thread_get_priority (void) {
	return thread_current ()->priority;
}

/* 이 구간의 동작과 의도를 설명한다. */
void
thread_set_nice (int nice UNUSED) {
	/* 이 구간의 동작과 의도를 설명한다. */
}

/* 이 구간의 동작과 의도를 설명한다. */
int
thread_get_nice (void) {
	/* 이 구간의 동작과 의도를 설명한다. */
	return 0;
}

/* 이 구간의 동작과 의도를 설명한다. */
int
thread_get_load_avg (void) {
	/* 이 구간의 동작과 의도를 설명한다. */
	return 0;
}

/* 이 구간의 동작과 의도를 설명한다. */
int
thread_get_recent_cpu (void) {
	/* 이 구간의 동작과 의도를 설명한다. */
	return 0;
}

/* 이 구간의 동작과 의도를 설명한다. */
static void
idle (void *idle_started_ UNUSED) {
	struct semaphore *idle_started = idle_started_;

	idle_thread = thread_current ();
	sema_up (idle_started);

	for (;;) {
		/* 이 구간의 동작과 의도를 설명한다. */
		intr_disable ();
		thread_block ();

		/* 이 구간의 동작과 의도를 설명한다. */
		asm volatile ("sti; hlt" : : : "memory");
	}
}

/* 이 구간의 동작과 의도를 설명한다. */
static void
kernel_thread (thread_func *function, void *aux) {
	ASSERT (function != NULL);

	intr_enable ();       /* 이 구간의 동작과 의도를 설명한다. */
	function (aux);       /* 이 구간의 동작과 의도를 설명한다. */
	thread_exit ();       /* 이 구간의 동작과 의도를 설명한다. */
}


/* 이 구간의 동작과 의도를 설명한다. */
static void
init_thread (struct thread *t, const char *name, int priority) {
	ASSERT (t != NULL);
	ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
	ASSERT (name != NULL);

	memset (t, 0, sizeof *t);
	t->status = THREAD_BLOCKED;
	strlcpy (t->name, name, sizeof t->name);
	t->tf.rsp = (uint64_t) t + PGSIZE - sizeof (void *);
	t->priority = priority;
	t->magic = THREAD_MAGIC;
}

/* 이 구간의 동작과 의도를 설명한다. */
static struct thread *
next_thread_to_run (void) {
	if (list_empty (&ready_list))
		return idle_thread;
	else
		return list_entry (list_pop_front (&ready_list), struct thread, elem);
}

/* 이 구간의 동작과 의도를 설명한다. */
void
do_iret (struct intr_frame *tf) {
	__asm __volatile(
			"movq %0, %%rsp\n"
			"movq 0(%%rsp),%%r15\n"
			"movq 8(%%rsp),%%r14\n"
			"movq 16(%%rsp),%%r13\n"
			"movq 24(%%rsp),%%r12\n"
			"movq 32(%%rsp),%%r11\n"
			"movq 40(%%rsp),%%r10\n"
			"movq 48(%%rsp),%%r9\n"
			"movq 56(%%rsp),%%r8\n"
			"movq 64(%%rsp),%%rsi\n"
			"movq 72(%%rsp),%%rdi\n"
			"movq 80(%%rsp),%%rbp\n"
			"movq 88(%%rsp),%%rdx\n"
			"movq 96(%%rsp),%%rcx\n"
			"movq 104(%%rsp),%%rbx\n"
			"movq 112(%%rsp),%%rax\n"
			"addq $120,%%rsp\n"
			"movw 8(%%rsp),%%ds\n"
			"movw (%%rsp),%%es\n"
			"addq $32, %%rsp\n"
			"iretq"
			: : "g" ((uint64_t) tf) : "memory");
}

/* 이 구간의 동작과 의도를 설명한다. */
static void
thread_launch (struct thread *th) {
	uint64_t tf_cur = (uint64_t) &running_thread ()->tf;
	uint64_t tf = (uint64_t) &th->tf;
	ASSERT (intr_get_level () == INTR_OFF);

	/* 이 구간의 동작과 의도를 설명한다. */
	__asm __volatile (
			/* 이 구간의 동작과 의도를 설명한다. */
			"push %%rax\n"
			"push %%rbx\n"
			"push %%rcx\n"
			/* 이 구간의 동작과 의도를 설명한다. */
			"movq %0, %%rax\n"
			"movq %1, %%rcx\n"
			"movq %%r15, 0(%%rax)\n"
			"movq %%r14, 8(%%rax)\n"
			"movq %%r13, 16(%%rax)\n"
			"movq %%r12, 24(%%rax)\n"
			"movq %%r11, 32(%%rax)\n"
			"movq %%r10, 40(%%rax)\n"
			"movq %%r9, 48(%%rax)\n"
			"movq %%r8, 56(%%rax)\n"
			"movq %%rsi, 64(%%rax)\n"
			"movq %%rdi, 72(%%rax)\n"
			"movq %%rbp, 80(%%rax)\n"
			"movq %%rdx, 88(%%rax)\n"
			"pop %%rbx\n"              // 한국어 주석
			"movq %%rbx, 96(%%rax)\n"
			"pop %%rbx\n"              // 한국어 주석
			"movq %%rbx, 104(%%rax)\n"
			"pop %%rbx\n"              // 한국어 주석
			"movq %%rbx, 112(%%rax)\n"
			"addq $120, %%rax\n"
			"movw %%es, (%%rax)\n"
			"movw %%ds, 8(%%rax)\n"
			"addq $32, %%rax\n"
			"call __next\n"         // 한국어 주석
			"__next:\n"
			"pop %%rbx\n"
			"addq $(out_iret -  __next), %%rbx\n"
			"movq %%rbx, 0(%%rax)\n" // 한국어 주석
			"movw %%cs, 8(%%rax)\n"  // 한국어 주석
			"pushfq\n"
			"popq %%rbx\n"
			"mov %%rbx, 16(%%rax)\n" // 한국어 주석
			"mov %%rsp, 24(%%rax)\n" // 한국어 주석
			"movw %%ss, 32(%%rax)\n"
			"mov %%rcx, %%rdi\n"
			"call do_iret\n"
			"out_iret:\n"
			: : "g"(tf_cur), "g" (tf) : "memory"
			);
}

/* 이 구간의 동작과 의도를 설명한다. */
static void
do_schedule(int status) {
	ASSERT (intr_get_level () == INTR_OFF);
	ASSERT (thread_current()->status == THREAD_RUNNING);
	while (!list_empty (&destruction_req)) {
		struct thread *victim =
			list_entry (list_pop_front (&destruction_req), struct thread, elem);
		palloc_free_page(victim);
	}
	thread_current ()->status = status;
	schedule ();
}

static void
schedule (void) {
	struct thread *curr = running_thread ();
	struct thread *next = next_thread_to_run ();

	ASSERT (intr_get_level () == INTR_OFF);
	ASSERT (curr->status != THREAD_RUNNING);
	ASSERT (is_thread (next));
	/* 이 구간의 동작과 의도를 설명한다. */
	next->status = THREAD_RUNNING;

	/* 이 구간의 동작과 의도를 설명한다. */
	thread_ticks = 0;

#ifdef USERPROG
	/* 이 구간의 동작과 의도를 설명한다. */
	process_activate (next);
#endif

	if (curr != next) {
		/* 이 구간의 동작과 의도를 설명한다. */
		if (curr && curr->status == THREAD_DYING && curr != initial_thread) {
			ASSERT (curr != next);
			list_push_back (&destruction_req, &curr->elem);
		}

		/* 이 구간의 동작과 의도를 설명한다. */
		thread_launch (next);
	}
}

/* 이 구간의 동작과 의도를 설명한다. */
static tid_t
allocate_tid (void) {
	static tid_t next_tid = 1;
	tid_t tid;

	lock_acquire (&tid_lock);
	tid = next_tid++;
	lock_release (&tid_lock);

	return tid;
}
