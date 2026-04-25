#include "devices/timer.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include "threads/interrupt.h"
#include "threads/io.h"
#include "threads/synch.h"
#include "threads/thread.h"

/* 이 구간의 동작과 의도를 설명한다. */

#if TIMER_FREQ < 19
#error 8254 timer requires TIMER_FREQ >= 19
#endif
#if TIMER_FREQ > 1000
#error TIMER_FREQ <= 1000 recommended
#endif

/* 이 구간의 동작과 의도를 설명한다. */
static int64_t ticks;

/* 이 구간의 동작과 의도를 설명한다. */
static unsigned loops_per_tick;

static intr_handler_func timer_interrupt;
static bool too_many_loops (unsigned loops);
static void busy_wait (int64_t loops);
static void real_time_sleep (int64_t num, int32_t denom);

/* 이 구간의 동작과 의도를 설명한다. */
void
timer_init (void) {
	/* 이 구간의 동작과 의도를 설명한다. */
	uint16_t count = (1193180 + TIMER_FREQ / 2) / TIMER_FREQ;

	outb (0x43, 0x34);    /* 이 구간의 동작과 의도를 설명한다. */
	outb (0x40, count & 0xff);
	outb (0x40, count >> 8);

	intr_register_ext (0x20, timer_interrupt, "8254 Timer");
}

/* 이 구간의 동작과 의도를 설명한다. */
void
timer_calibrate (void) {
	unsigned high_bit, test_bit;

	ASSERT (intr_get_level () == INTR_ON);
	printf ("Calibrating timer...  ");

	/* 이 구간의 동작과 의도를 설명한다. */
	loops_per_tick = 1u << 10;
	while (!too_many_loops (loops_per_tick << 1)) {
		loops_per_tick <<= 1;
		ASSERT (loops_per_tick != 0);
	}

	/* 이 구간의 동작과 의도를 설명한다. */
	high_bit = loops_per_tick;
	for (test_bit = high_bit >> 1; test_bit != high_bit >> 10; test_bit >>= 1)
		if (!too_many_loops (high_bit | test_bit))
			loops_per_tick |= test_bit;

	printf ("%'"PRIu64" loops/s.\n", (uint64_t) loops_per_tick * TIMER_FREQ);
}

/* 이 구간의 동작과 의도를 설명한다. */
int64_t
timer_ticks (void) {
	enum intr_level old_level = intr_disable ();
	int64_t t = ticks;
	intr_set_level (old_level);
	barrier ();
	return t;
}

/* 이 구간의 동작과 의도를 설명한다. */
int64_t
timer_elapsed (int64_t then) {
	return timer_ticks () - then;
}

/* 이 구간의 동작과 의도를 설명한다. */
void
timer_sleep (int64_t ticks) {
	int64_t start;

	ASSERT (intr_get_level () == INTR_ON);
	if (ticks <= 0) {
		return;
	}
	start = timer_ticks ();
	thread_sleep (start + ticks);
}


/* 이 구간의 동작과 의도를 설명한다. */
void
timer_msleep (int64_t ms) {
	real_time_sleep (ms, 1000);
}

/* 이 구간의 동작과 의도를 설명한다. */
void
timer_usleep (int64_t us) {
	real_time_sleep (us, 1000 * 1000);
}

/* 이 구간의 동작과 의도를 설명한다. */
void
timer_nsleep (int64_t ns) {
	real_time_sleep (ns, 1000 * 1000 * 1000);
}

/* 이 구간의 동작과 의도를 설명한다. */
void
timer_print_stats (void) {
	printf ("Timer: %"PRId64" ticks\n", timer_ticks ());
}


/* 이 구간의 동작과 의도를 설명한다. */
static void
timer_interrupt (struct intr_frame *args UNUSED) {
	ticks++;
	thread_awake (ticks);
	thread_tick ();
}

/* 이 구간의 동작과 의도를 설명한다. */
static bool
too_many_loops (unsigned loops) {
	/* 이 구간의 동작과 의도를 설명한다. */
	int64_t start = ticks;
	while (ticks == start)
		barrier ();

	/* 이 구간의 동작과 의도를 설명한다. */
	start = ticks;
	busy_wait (loops);

	/* 이 구간의 동작과 의도를 설명한다. */
	barrier ();
	return start != ticks;
}

/* 이 구간의 동작과 의도를 설명한다. */
static void NO_INLINE
busy_wait (int64_t loops) {
	while (loops-- > 0)
		barrier ();
}

/* 이 구간의 동작과 의도를 설명한다. */
static void
real_time_sleep (int64_t num, int32_t denom) {
	/* 이 구간의 동작과 의도를 설명한다. */
	int64_t ticks = num * TIMER_FREQ / denom;

	ASSERT (intr_get_level () == INTR_ON);
	if (ticks > 0) {
		/* 이 구간의 동작과 의도를 설명한다. */
		timer_sleep (ticks);
	} else {
		/* 이 구간의 동작과 의도를 설명한다. */
		ASSERT (denom % 1000 == 0);
		busy_wait (loops_per_tick * num / 1000 * TIMER_FREQ / (denom / 1000));
	}
}
