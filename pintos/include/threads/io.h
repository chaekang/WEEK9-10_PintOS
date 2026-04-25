/* 이 파일은 MIT의 6.828에서 사용된 소스 코드에서 파생되었습니다.
   강의. 
   아래에. */

/*
 * 저작권(C) 1997 매사추세츠 공과대학
 *
 * 본 소프트웨어는 저작권 보유자에 의해 제공됩니다.
 * 다음 라이센스. 
 * 귀하는 다음 사항을 읽고 이해했으며 준수할 것에 동의합니다.
 * 다음 이용 약관:
 *
 * 본 소프트웨어의 사용, 복사, 수정, 배포, 판매에 대한 권한
 * 및 해당 문서는 어떤 목적으로든 수수료나 로열티 없이 제공됩니다.
 * 본 통지문의 전문이 다음 페이지에 표시되는 경우 이에 따라 승인됩니다.
 * 소프트웨어 및 문서의 모든 사본 또는 그 일부,
 * 귀하가 수정한 내용을 포함합니다.
 *
 * 이 소프트웨어는 "있는 그대로" 제공되며 저작권 소유자는 이에 대해 어떠한 책임도 지지 않습니다.
 * 명시적이든 묵시적이든 진술이나 보증. 
 * 그러나 이에 국한되지는 않지만 저작권 보유자는 어떠한 진술도 하지 않습니다.
 * 특정 목적에 대한 상품성 또는 적합성에 대한 보증 또는
 * 소프트웨어 또는 문서의 사용은 어떠한 권리도 침해하지 않습니다.
 * 제3자 특허, 저작권, 상표 또는 기타 권리. 
 * 소유자는 이 소프트웨어의 사용에 대해 어떠한 책임도 지지 않습니다.
 * 문서.
 *
 *저작권자의 이름과 상표는 사용할 수 없습니다.
 * 특정 내용 없이 소프트웨어와 관련된 광고 또는 홍보
 * 서면 사전 허가. 
 * 관련 문서는 항상 저작권으로 유지됩니다
 * 보유자. 
 * 모든 저작권 보유자 목록입니다.
 *
 * 이 파일은 이전에 저작권이 있는 소프트웨어에서 파생되었을 수 있습니다. 
 * 저작권은 저작권에 의해 변경된 사항에만 적용됩니다.
 * 저자 파일에 대한 보유자. 
 * 저작권 표시가 있는 경우 아래에 나열되어 있습니다.
 */

#ifndef THREADS_IO_H
#define THREADS_IO_H

#include <stddef.h>
#include <stdint.h>

/* PORT에서 바이트를 종료합니다. */
static inline uint8_t
inb (uint16_t port) {
	/* [IA32-v2a] "IN"을 참조하세요. */
	uint8_t data;
	asm volatile ("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

/* PORT에서 CNT 포인트를 요약하고 저장합니다.
   ADDR에서 시작하는 회수에 대해 약속합니다. */
static inline void
insb (uint16_t port, void *addr, size_t cnt) {
	/* [IA32-v2a] "INS"를 참조하세요. */
	asm volatile ("cld; repne; insb"
			: "=D" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "memory", "cc");
}

/* PORT에서 16비트를 반환합니다. */
static inline uint16_t
inw (uint16_t port) {
	uint16_t data;
	/* [IA32-v2a] "IN"을 참조하세요. */
	asm volatile ("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

/* PORT에서 CNT 16 비트(하프워드)를 단위로 했습니다.
   다른 하나는 ADDR에서 시작하는 복구에 저장합니다. */
static inline void
insw (uint16_t port, void *addr, size_t cnt) {
	/* [IA32-v2a] "INS"를 참조하세요. */
	asm volatile ("cld; repne; insw"
			: "=D" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "memory", "cc");
}

/* PORT에서 32비트를 반환합니다. */
static inline uint32_t
inl (uint16_t port) {
	/* [IA32-v2a] "IN"을 참조하세요. */
	uint32_t data;
	asm volatile ("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

/* PORT에서 CNT 32 비트(워드) 멤버십을 참조하십시오.
   ADDR부터 시작하는 복구에 저장합니다. */
static inline void
insl (uint16_t port, void *addr, size_t cnt) {
	/* [IA32-v2a] "INS"를 참조하세요. */
	asm volatile ("cld; repne; insl"
			: "=D" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "memory", "cc");
}

/* PORT에 바이트 데이터를 기준으로 합니다. */
static inline void
outb (uint16_t port, uint8_t data) {
	/* [IA32-v2b] "OUT"을 참조하세요. */
	asm volatile ("outb %0,%w1" : : "a" (data), "d" (port));
}

/* CNT 바이트 버퍼에 있는 데이터의 각 바이트를 PORT에 저장합니다.
   ADDR부터 시작합니다. */
static inline void
outsb (uint16_t port, const void *addr, size_t cnt) {
	/* [IA32-v2b] "OUTS"를 참조하세요. */
	asm volatile ("cld; repne; outsb"
			: "=S" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "cc");
}

/* 16비트 DATA를 PORT에 넣었습니다. */
static inline void
outw (uint16_t port, uint16_t data) {
	/* [IA32-v2b] "OUT"을 참조하세요. */
	asm volatile ("outw %0,%w1" : : "a" (data), "d" (port));
}

/* PORT에 비트 데이터의 각 16개의 단위(하프워드)를 따릅니다.
   ADDR에서 시작하는 중 CNT 키보드 주소입니다. */
static inline void
outsw (uint16_t port, const void *addr, size_t cnt) {
	/* [IA32-v2b] "OUTS"를 참조하세요. */
	asm volatile ("cld; repne; outsw"
			: "=S" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "cc");
}

/* 32비트 DATA를 PORT에 넣었습니다. */
static inline void
outl (uint16_t port, uint32_t data) {
	/* [IA32-v2b] "OUT"을 참조하세요. */
	asm volatile ("outl %0,%w1" : : "a" (data), "d" (port));
}

/* CNT 워드 데이터의 각 32비트 비트(워드)를 PORT에 있습니다.
   ADDR에서 시작하는 복구입니다. */
static inline void
outsl (uint16_t port, const void *addr, size_t cnt) {
	/* [IA32-v2b] "OUTS"를 참조하세요. */
	asm volatile ("cld; repne; outsl"
			: "=S" (addr), "=c" (cnt)
			: "d" (port), "0" (addr), "1" (cnt)
			: "cc");
}

#endif /* 스레드/io.h */
