#ifndef THREADS_INIT_H
#define THREADS_INIT_H

#include <debug.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 이 구간의 동작과 의도를 설명한다. */
extern size_t ram_pages;

/* 이 구간의 동작과 의도를 설명한다. */
extern uint64_t *base_pml4;

/* 이 구간의 동작과 의도를 설명한다. */
extern bool power_off_when_done;

void power_off (void) NO_RETURN;

#endif /* 이 구간의 동작과 의도를 설명한다. */
