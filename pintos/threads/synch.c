/* 이 파일은 Nachos의 소스 코드에서 파생되었습니다.
   교육용으로. 
   아래 전체로 재현됩니다. */

/* 저작권 (c) 1992-1996 The Regents of the University of California.
   모든 권리 보유.

   이 소프트웨어의 사용, 복사, 수정 및 배포 권한
   수수료 없이 어떤 목적으로든 서류를 제출해야 하며,
   서면 동의 없이 본 계약에 따라
   위의 저작권 고지 사항과 다음 두 단락이 나타납니다.
   이 소프트웨어의 모든 사본에서.

   어떠한 경우에도 캘리포니아 대학교는
   직간접적, 특수적, 부수적, 또는
   본 소프트웨어의 사용으로 인해 발생하는 결과적 손해
   그리고 그 문서는 캘리포니아 대학이
   그러한 손상의 가능성에 대해 통보받았습니다.

   캘리포니아 대학교는 특히
   묵시적 보증을 포함하되 이에 국한되지 않는 보증
   특정 상품에 대한 상품성 및 적합성에 대한 보증
   목적. 본 계약에 따라 제공되는 소프트웨어는 "있는 그대로"
   기초 및 캘리포니아 대학은 다음과 같은 의무가 없습니다.
   유지 보수, 지원, 업데이트, 개선 사항 제공 또는
   수정
   */

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* 세마포어 SEMA를 기본적으로 PDF로 사용합니다. 
   에 대한 두 개의 원자 연산자와 함께 음이 아닌 정수
   그것을 조작하는 것:

   - 아래로 또는 "P": 값이 양이 될 때까지 다음
   감소시킵니다.

   - up 또는 "V": 값을 증가시키다(그리고 대기 중 하나를 깨뜨리다).
   스레드 (있는 경우). */
void
sema_init (struct semaphore *sema, unsigned value) {
	ASSERT (sema != NULL);

	sema->value = value;
	list_init (&sema->waiters);
}

/* 세마포어에서 다운 또는 "P" 작업. 
   양수가 된 다음 원자적으로 감소시킵니다.

   이 함수는 절전 모드일 수 있으므로
   인터럽트 핸들러입니다. 이 함수는
   인터럽트가 비활성화되었지만 잠자기 상태이면 다음 예약된
   스레드는 아마도 인터럽트를 다시 켤 것입니다. 이것은
   sema_down 함수. */
void
sema_down (struct semaphore *sema) {
	enum intr_level old_level;

	ASSERT (sema != NULL);
	ASSERT (!intr_context ());

	old_level = intr_disable ();
	while (sema->value == 0) {
		list_push_back (&sema->waiters, &thread_current ()->elem);
		thread_block ();
	}
	sema->value--;
	intr_set_level (old_level);
}

/* 세마포어에서 아래로 또는 "P" 작업, 하지만 경우에만
   세마포어는 이미 0이 아닙니다. 
   감소, 그렇지 않으면 거짓.

   이 함수는 인터럽트 핸들러에서 호출될 수 있다. */
bool
sema_try_down (struct semaphore *sema) {
	enum intr_level old_level;
	bool success;

	ASSERT (sema != NULL);

	old_level = intr_disable ();
	if (sema->value > 0)
	{
		sema->value--;
		success = true;
	}
	else
		success = false;
	intr_set_level (old_level);

	return success;
}

/* 세마포어에서 또는 "V" 상승. 
   sEMA를 기다리는 사람들 중 한 명을 깨뜨리고 있습니다.

   이 함수는 인터럽트 핸들러에서 호출될 수 있다. */
void
sema_up (struct semaphore *sema) {
	enum intr_level old_level;

	ASSERT (sema != NULL);

	old_level = intr_disable ();
	if (!list_empty (&sema->waiters))
		thread_unblock (list_entry (list_pop_front (&sema->waiters),
					struct thread, elem));
	sema->value++;
	intr_set_level (old_level);
}

static void sema_test_helper (void *sema_);

/* 제어 "탁구" 를 만드는 세마포어에 대한 자체 테스트
   한쌍의 스레드 사이입니다. 
   무슨 일이 일어나고 있나요? */
void
sema_self_test (void) {
	struct semaphore sema[2];
	int i;

	printf ("Testing semaphores...");
	sema_init (&sema[0], 0);
	sema_init (&sema[1], 0);
	thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
	for (i = 0; i < 10; i++)
	{
		sema_up (&sema[0]);
		sema_down (&sema[1]);
	}
	printf ("done.\n");
}

/* Sema_self_test ()에서 사용하는 쓰레드 함수입니다. */
static void
sema_test_helper (void *sema_) {
	struct semaphore *sema = sema_;
	int i;

	for (i = 0; i < 10; i++)
	{
		sema_down (&sema[0]);
		sema_up (&sema[1]);
	}
}

/* 자물쇠를 초기화합니다. 자물쇠는 최대 한 개까지 보유할 수 있습니다.
   언제든지 스레드. 우리의 자물쇠는 "재귀적" 이 아니라
   현재 잠금을 유지하고 있는 스레드에 대한 오류입니다.
   그 자물쇠를 획득하려고 노력하십시오.

   자물쇠는 이니셜이 있는 세마포어의 전문화입니다.
   값 1. 잠금 장치와 이러한 잠금 장치의 차이점은
   세마포어는 2배입니다. 첫째, 세마포어는 값을 가질 수 있습니다.
   1보다 크지만 잠금 장치는 단일 사용자만 소유할 수 있습니다.
   한 번에 스레드. 둘째, 세마포에는 소유자가 없습니다.
   즉, 하나의 스레드가 세마포어를 "다운" 한 다음
   다른 사람이 그것을 "위로" 하지만 자물쇠가있는 동일한 스레드는 둘 다
   획득하고 해제합니다. 이러한 제한이 증명될 때
   세마포를 사용해야 한다는 좋은 신호입니다.
   자물쇠 대신에 말이죠. */
void
lock_init (struct lock *lock) {
	ASSERT (lock != NULL);

	lock->holder = NULL;
	sema_init (&lock->semaphore, 1);
}

/* 자물쇠를 획득하여 다음 경우에 사용할 수 있을 때까지 잔다.
   필요. 잠금 장치는 이미 전류에 의해 유지되어서는 안됩니다.
   파악할 수 있으니까요.

   이 함수는 절전 모드일 수 있으므로
   인터럽트 핸들러입니다. 이 함수는
   인터럽트가 비활성화되었지만 인터럽트가 다시 켜지면
   우리는 잠을 자야 합니다. */
void
lock_acquire (struct lock *lock) {
	ASSERT (lock != NULL);
	ASSERT (!intr_context ());
	ASSERT (!lock_held_by_current_thread (lock));

	sema_down (&lock->semaphore);
	lock->holder = thread_current ();
}

/* 잠금을 획득하려고 시도하고 성공하거나 실패하면 true를 반환합니다.
   실패시. 잠금 장치는 이미 전류에 의해 유지되어서는 안됩니다.
   파악할 수 있으니까요.

   이 함수는 절전 모드가 아니므로
   레지스트리에서 */
bool
lock_try_acquire (struct lock *lock) {
	bool success;

	ASSERT (lock != NULL);
	ASSERT (!lock_held_by_current_thread (lock));

	success = sema_try_down (&lock->semaphore);
	if (success)
		lock->holder = thread_current ();
	return success;
}

/* 현재 스레드가 소유해야 하는 잠금을 해제합니다.
   LOCK_RELEASE 함수입니다.

   인터럽트 핸들러는 잠금을 획득할 수 없으므로
   인터럽트 내에서 잠금을 해제하는 것이 합리적입니다.
   취급자인 */
void
lock_release (struct lock *lock) {
	ASSERT (lock != NULL);
	ASSERT (lock_held_by_current_thread (lock));

	lock->holder = NULL;
	sema_up (&lock->semaphore);
}

/* 현재 스레드가 유지되는 상태로 유지되며 true, false를 유지합니다.
   그렇지 않으면. (다른 스레드가 유지되는지 여부를 테스트하는 데 유의하십시오.
   자물쇠는 거추장스러울 것이다.) */
bool
lock_held_by_current_thread (const struct lock *lock) {
	ASSERT (lock != NULL);

	return lock->holder == thread_current ();
}

/* 목록에 세마포가 하나 있습니다. */
struct semaphore_elem {
	struct list_elem elem;              /* 요소 나열. */
	struct semaphore semaphore;         /* 이 세마포어. */
};

/* 조건을 만족시키세요. 
   하나의 코드로 조건을 알리고 협조할 수 있습니다
   신호를 수신하고 이에 따라 조치를 취하는 코드입니다. */
void
cond_init (struct condition *cond) {
	ASSERT (cond != NULL);

	list_init (&cond->waiters);
}

/* LOCK을 원자로 개방하고 COND가 다음의 신호를 받을 때까지 기다립니다.
   다른 코드 조각.  
   돌아오기 전에 다시 획득했습니다.  
   이 기능.

   이 기능으로 구현된 모니터는 "Mesa" 스타일이 아닌 "Mesa" 스타일입니다.
   "Hoare" 스타일, 즉 신호를 전달하는 것은
   원자 연산.  
   대기가 완료된 후의 조건 및 필요한 경우 대기
   다시.

   주어진 조건 변수는 단일 조건과만 연관됩니다.
   그러나 하나의 잠금은 여러 개의 잠금과 연관될 수 있습니다.
   조건변수.  
   잠금에서 조건 변수까지.

   이 함수는 절전 모드일 수 있으므로
   인터럽트 핸들러입니다. 이 함수는
   인터럽트가 비활성화되었지만 인터럽트가 다시 켜지면
   우리는 잠을 자야 합니다. */
void
cond_wait (struct condition *cond, struct lock *lock) {
	struct semaphore_elem waiter;

	ASSERT (cond != NULL);
	ASSERT (lock != NULL);
	ASSERT (!intr_context ());
	ASSERT (lock_held_by_current_thread (lock));

	sema_init (&waiter.semaphore, 0);
	list_push_back (&cond->waiters, &waiter.elem);
	lock_release (lock);
	sema_down (&waiter.semaphore);
	lock_acquire (lock);
}

/* 스레드가 COND(LOCK으로 보호됨)를 기다리고 있는 경우
   이 함수는 그 중 하나에게 대기 상태에서 깨어나도록 신호를 보냅니다.
   이 호출을 요청하기 전에 잠금을 유지해야 합니다.

   인터럽트 핸들러는 잠금을 획득할 수 없으므로
   내에서 조건 변수에 신호를 보내는 것이 합리적입니다.
   레지스트리에서 */
void
cond_signal (struct condition *cond, struct lock *lock UNUSED) {
	ASSERT (cond != NULL);
	ASSERT (lock != NULL);
	ASSERT (!intr_context ());
	ASSERT (lock_held_by_current_thread (lock));

	if (!list_empty (&cond->waiters))
		sema_up (&list_entry (list_pop_front (&cond->waiters),
					struct semaphore_elem, elem)->semaphore);
}

/* COND를 기다리고 있는 모든 스레드를 이해하고 있습니다(다음으로 보호됨).
   잠그다).  

   인터럽트 핸들러는 잠금을 획득할 수 없으므로
   내에서 조건 변수에 신호를 보내는 것이 합리적입니다.
   레지스트리에서 */
void
cond_broadcast (struct condition *cond, struct lock *lock) {
	ASSERT (cond != NULL);
	ASSERT (lock != NULL);

	while (!list_empty (&cond->waiters))
		cond_signal (cond, lock);
}
