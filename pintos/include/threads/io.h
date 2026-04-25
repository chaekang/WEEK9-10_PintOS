/* 이 구간의 동작과 의도를 설명한다. */

/* 이 구간의 동작과 의도를 설명한다. */

#ifndef THREADS_IO_H
#define THREADS_IO_H

#include <stddef.h>
#include <stdint.h>

/* 이 구간의 동작과 의도를 설명한다. */
static inline uint8_t
inb (uint16_t port) {
	/* 이 구간의 동작과 의도를 설명한다. */
	uint8_t data;
	asm volatile ("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
insb (uint16_t port, void *addr, size_t cnt) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("cld; repne; insb"
			: "=D" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "memory", "cc");
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline uint16_t
inw (uint16_t port) {
	uint16_t data;
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
insw (uint16_t port, void *addr, size_t cnt) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("cld; repne; insw"
			: "=D" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "memory", "cc");
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline uint32_t
inl (uint16_t port) {
	/* 이 구간의 동작과 의도를 설명한다. */
	uint32_t data;
	asm volatile ("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
insl (uint16_t port, void *addr, size_t cnt) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("cld; repne; insl"
			: "=D" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "memory", "cc");
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
outb (uint16_t port, uint8_t data) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("outb %0,%w1" : : "a" (data), "d" (port));
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
outsb (uint16_t port, const void *addr, size_t cnt) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("cld; repne; outsb"
			: "=S" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "cc");
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
outw (uint16_t port, uint16_t data) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("outw %0,%w1" : : "a" (data), "d" (port));
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
outsw (uint16_t port, const void *addr, size_t cnt) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("cld; repne; outsw"
			: "=S" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "cc");
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
outl (uint16_t port, uint32_t data) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("outl %0,%w1" : : "a" (data), "d" (port));
}

/* 이 구간의 동작과 의도를 설명한다. */
static inline void
outsl (uint16_t port, const void *addr, size_t cnt) {
	/* 이 구간의 동작과 의도를 설명한다. */
	asm volatile ("cld; repne; outsl"
			: "=S" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "cc");
}

#endif /* 이 구간의 동작과 의도를 설명한다. */
