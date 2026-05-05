#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "userprog/process.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "intrinsic.h"
#include "devices/input.h"
#include "filesys/file.h"
#include "threads/synch.h"



void syscall_entry (void);
void syscall_handler (struct intr_frame *);
static struct lock filesys_lock;
static struct fd_entry *find_fd_entry(int fd);

/* 시스템 호출.
 *
 * 예전에는 시스템 호출 서비스를 인터럽트 핸들러가 처리했다
 * (예: Linux의 int 0x80). 하지만 x86-64에서는 CPU 제조사가 시스템 호출을
 * 요청하는 더 효율적인 경로로 `syscall` 명령어를 제공한다.
 *
 * syscall 명령어는 모델 전용 레지스터(MSR)에 저장된 값을 읽어 동작한다.
 * 자세한 내용은 매뉴얼을 참고하라. */

#define MSR_STAR 0xc0000081         /* 세그먼트 셀렉터 MSR */
#define MSR_LSTAR 0xc0000082        /* 롱 모드 SYSCALL 대상 */
#define MSR_SYSCALL_MASK 0xc0000084 /* eflags용 마스크 */

void
syscall_init (void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* syscall_entry가 사용자 영역 스택을 커널 모드 스택으로 바꾸기 전까지는
	 * 인터럽트 서비스 루틴이 어떤 인터럽트도 처리하면 안 된다. 그래서
	 * FLAG_FL을 마스킹한다. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
	list_init(&filesys_lock);
}


/* user가 요청한 fd를 읽어 buffer에 내용을 저장 */
int read(int fd, void *buffer, unsigned size) {
	/* 인자 기본 검사 */
	if (fd < 0) {
		return -1;
	}

	if (fd == 1) { 	// fd == 1: 실패
		return -1;
	}
	// size 검사
		if (size == 0) {
		return 0;
	}

	/* buffer 유효성 검사 */
	if (buffer == NULL) { // 버퍼 시작 주소 검사
		return exit(-1);
	}
	char *start = buffer; 	// buffer 전체 범위 검사
	char *end = start + size -1;
	char *p= buffer;

	for (p = start; p <= end; p++) {
		if (is_user_vaddr(p) && pml4_get_page(thread_current()->pml4, p) ) { // p가 사용자 주소 범위 안인지 + p가 페이지 테이블에 매핑되어 있는지 검사
			continue;
		}
		else {
			return exit(-1); // exit syscall 구현 에정
		}
	}

	/* 실제 read를 수행 */
	size_t i;
	char *buf = buffer;

	if (fd == 0) { // stdin이면 input_getc로 buffer에 size만큼 쓰기
		for (i=0; i<size; i++) {
			buf[i] = input_getc();
		}
		return size;
	}
	if (fd >= 2) {
		struct fd_entry *entry = find_fd_entry(fd); // file이면 fd로 entry를 찾는다
		if (entry == NULL || entry->file == NULL) {
			return -1;
		}
		lock_acquire(&filesys_lock); // file이면 filesys lock 획득 후 file_read
		off_t read_size = file_read(entry->file, buffer, size);
		lock_release(&filesys_lock); // file이면 filesys lock 해제

		return read_size; // 읽은 바이트 수 반환
	}
}

/* fd를 받아 fd_entry를 찾아서 반환한다 */
static struct fd_entry *
find_fd_entry(int fd) {
	struct thread *curr = thread_current();
	struct list_elem *e;

	for (e = list_begin(&curr->fd_list);
		 e != list_end(&curr->fd_list);
		 e = list_next(e)) {
		struct fd_entry *entry = list_entry(e, struct fd_entry, elem);

		if (entry->fd == fd) {
			return entry;
		}
	}
	return NULL;
}


/* 메인 시스템 호출 인터페이스 */
void
syscall_handler (struct intr_frame *f UNUSED) {
	// TODO: Your implementation goes here.

	struct thread *t = thread_current();

	switch (f->R.rax)
	{
	case SYS_EXIT: {
		uint64_t status = f->R.rdi;
		//t->exit_status = status;
		thread_exit ();
		break;
	}

	case SYS_WRITE: {
		if ((int) f->R.rdi == 1) {
			putbuf((const char *) f->R.rsi, (size_t) f->R.rdx);
			f->R.rax = f->R.rdx;
		}
		break;
	}
	
	case SYS_HALT: {
		power_off();
	}
	
	default:
		break;
	}
}
