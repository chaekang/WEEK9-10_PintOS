#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include "filesys/filesys.h"
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "userprog/process.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "intrinsic.h"
#include "threads/vaddr.h"
#include "threads/mmu.h"

void syscall_entry (void);
void syscall_handler (struct intr_frame *);

static void validate_user_ptr(const void *ptr);
static void validate_user_buffer(const void *buffer, size_t size);
static void validate_user_string(const char *str);
static void kill_process_due_to_bad_user_memory(void);
static struct fd_entry * find_fd_entry (int fd, struct list *fd_table);
static struct fd_entry * create_fd_entry (struct file* file);
static void remove_fd_entry (struct fd_entry *entry);

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
}

/* 메인 시스템 호출 인터페이스 */
void
syscall_handler (struct intr_frame *f UNUSED) {
	// TODO: Your implementation goes here.
	struct thread *t = thread_current();

	switch (f->R.rax)
	{
	case SYS_OPEN: {
		const char * filename = (const char *)f->R.rdi;
		validate_user_string(filename);
		struct file *file = filesys_open(filename);
		if (file == NULL) {
			f->R.rax = -1;
		} else {
			struct fd_entry *fd_entry = create_fd_entry(file);
			if (fd_entry == NULL) {
				file_close(file);
				f->R.rax = -1;
			} else {
				list_push_back(&thread_current()->fd_table, &fd_entry->elem);
				f->R.rax = fd_entry->fd;
			}
		}
		break;
	}

	case SYS_CREATE: {
		const char * filename = (const char *)f->R.rdi;
		size_t initial_size = f->R.rsi;
		validate_user_string(filename);
		f->R.rax = filesys_create(filename, initial_size);
		break;
	}

	case SYS_EXIT: {
		uint64_t status = f->R.rdi;
		t->exit_status = status;
		thread_exit ();
		break;
	}

	case SYS_WRITE: {
		int fd = (int) f->R.rdi;
		const void * buffer = (const void *)f->R.rsi;
		size_t size = f->R.rdx;

		if (fd == 1) {
			validate_user_buffer(buffer, size);
			putbuf(buffer, (size_t) f->R.rdx);
			f->R.rax = size;
		} else {
			struct fd_entry *fd_entry = find_fd_entry(fd, &thread_current()->fd_table);
			if (fd_entry == NULL) {
				f->R.rax = -1;
			} else {
				validate_user_buffer(buffer, size);
				f->R.rax = file_write(fd_entry->file, buffer, size);
			}
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

static void validate_user_ptr(const void *ptr) {
	if	(ptr == NULL) {
		kill_process_due_to_bad_user_memory();
	} else if (!is_user_vaddr(ptr)) {
		kill_process_due_to_bad_user_memory();
	} else if (pml4_get_page(thread_current()->pml4, ptr) == NULL) {
		kill_process_due_to_bad_user_memory();
	}
}

static void validate_user_buffer(const void *buffer, size_t size){
	if (size == 0) {
		return;
	} else {
		uint8_t *p = (uint8_t *) buffer;
		for (size_t i = 0; i < size; i++) {
			validate_user_ptr(p + i);
		}
	}
}

static void validate_user_string(const char *str) {
	if (str == NULL){
		kill_process_due_to_bad_user_memory();
	} else {
		const char *p = str;
		while (true) {
			validate_user_ptr(p);
			if (*p == '\0') {
				break;
			}
			p++;
		}
	}
}

static void kill_process_due_to_bad_user_memory(void) {
	thread_current()->exit_status = -1;
	thread_exit();
}

static struct fd_entry * find_fd_entry (int fd, struct list *fd_table) {
	struct list_elem *e;
	for (e = list_begin(fd_table); e != list_end(fd_table); e = list_next(e)) {
		struct fd_entry *fd_entry = list_entry(e, struct fd_entry, elem);
		if (fd_entry->fd == fd) {
			return fd_entry;
		}
	}
	return NULL;
}

static struct fd_entry * create_fd_entry (struct file* file) {
	struct fd_entry *entry = malloc(sizeof(struct fd_entry));
	if (entry == NULL) {
		return NULL;
	}
	entry->fd = thread_current()->next_fd;
	entry->file = file;
	thread_current()->next_fd++;
	return entry;
}

static void remove_fd_entry (struct fd_entry *entry) {
	list_remove(&entry->elem);
	file_close(entry->file);
	free(entry);
}
