#ifndef THREADS_INIT_H
#define THREADS_INIT_H

#include <debug.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 실제 메모리 크기(4KB 페이지)입니다. */
extern size_t ram_pages;

/* 커널 매핑만 포함된 페이지 맵 레벨 4. */
extern uint64_t *base_pml4;

/* -q: 커널 작업이 완료되면 전원을 끄시겠습니까? */
extern bool power_off_when_done;

void power_off (void) NO_RETURN;

#endif /* 스레드/init.h */
