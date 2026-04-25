#ifndef __LIB_KERNEL_LIST_H
#define __LIB_KERNEL_LIST_H

/* 이중 연결 리스트.
 *
 * 이중 연결 리스트의 구현에는 다음이 필요하지 않습니다.
 * 동적으로 할당된 메모리를 사용합니다.  
 * 처리인 목록 요소는 list_elem 구조를 포함해야 합니다.
 * 회원.  
 * list_elem's.  
 * struct list_elem을 포함하고 있기 때문에 견고합니다.

 * 예를 들어 `구조' 목록이 필요하다고 가정해 주어야 합니다.
 * 푸'.  
 * 회원은 다음과 같습니다.

 * 구조체 foo {
 * 구조 list_elem elem;
 * 정수 바;
 * ...다른 멤버들...
 * };

 * 그런 다음 `struct foo' 목록을 축하하고 대피할 수 있습니다.
 * 다음과 같습니다:

 * 구조 목록 foo_list;

 * list_init(&foo_list);

 * 반복은 다음이 필요한 일반적인 상황입니다.
 * 기억에 남는 list_elem을 다시 탐험하는 것은 기억으로 변환됩니다.
 * 구조.  

 * 기억 list_elem *e;

 * for (e = list_begin (&foo_list); e != list_end (&foo_list);
 * e = list_next (e)) {
 * struct foo *f = list_entry (e, struct foo, elem);
 * ...f로 뭔가를 해보세요...
 * }

 * 목록 전체에서 실제 목록 사용 예를 찾을 수 있습니다.
 * 원천; 
 * 스레드 디렉토리는 모두 목록을 사용합니다.

 * 이 목록의 인터페이스는 list<> 폴더에서 구성되었습니다.
 * C++ STL에서.  
 * 사용하기 쉽습니다.  
 * 이 목록은 유형 검사를 *아무* 수행하지 않으며 다른 많은 작업을 수행할 수 없습니다.
 * 정확성 검사.  

 * 목록 용어집:

 * - "앞": 목록의 첫 번째 요소입니다.  
 * 빈 목록.  

 * - "뒤로": 목록의 마지막 요소입니다.  
 * 목록.  

 * - "tail": 비유적으로 마지막 요소 바로 뒤의 요소
 * 목록의 요소.  
 * list_end()에 의해 반환됩니다.  
 * 앞에서 뒤로 반복합니다.

 * - "시작": 비어 있지 않은 목록에서 맨 앞.  
 * 목록, 꼬리.  
 * 앞에서 뒤로 반복의 시작점입니다.

 * - "head": 비유적으로 첫 번째 바로 앞의 요소
 * 목록의 요소.  
 * list_rend()에 의해 반환됩니다.  
 * 뒤에서 앞으로 반복합니다.

 * - "역방향 시작": 비어 있지 않은 목록에서 뒤.  
 * 빈 목록, 머리.  
 * 뒤에서 앞으로 반복하는 시작점입니다.
 *
 * - "내부 요소": 머리가 아닌 요소 또는
 * tail, 즉 실제 목록 요소입니다.  
 * 인테리어 요소가 없습니다.*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 목록 요소. */
struct list_elem {
	struct list_elem *prev;     /* 이전 목록 요소입니다. */
	struct list_elem *next;     /* 다음 목록 요소입니다. */
};

/* 목록. */
struct list {
	struct list_elem head;      /* 목록 머리. */
	struct list_elem tail;      /* 꼬리를 나열하십시오. */
};

/* 목록 요소 LIST_ELEM에 대한 포인터를 포인터로 변환합니다.
   LIST_ELEM이 내부에 포함된 구조입니다.  
   외부 구조 STRUCT의 이름과 기초 이름 MEMBER
   목록 요소의  
   예시 파일입니다. */
#define list_entry(LIST_ELEM, STRUCT, MEMBER)           \
	((STRUCT *) ((uint8_t *) &(LIST_ELEM)->next     \
		- offsetof (STRUCT, MEMBER.next)))

void list_init (struct list *);

/* 순회를 나열합니다. */
struct list_elem *list_begin (struct list *);
struct list_elem *list_next (struct list_elem *);
struct list_elem *list_end (struct list *);

struct list_elem *list_rbegin (struct list *);
struct list_elem *list_prev (struct list_elem *);
struct list_elem *list_rend (struct list *);

struct list_elem *list_head (struct list *);
struct list_elem *list_tail (struct list *);

/* 목록 삽입. */
void list_insert (struct list_elem *, struct list_elem *);
void list_splice (struct list_elem *before,
		struct list_elem *first, struct list_elem *last);
void list_push_front (struct list *, struct list_elem *);
void list_push_back (struct list *, struct list_elem *);

/* 목록 제거. */
struct list_elem *list_remove (struct list_elem *);
struct list_elem *list_pop_front (struct list *);
struct list_elem *list_pop_back (struct list *);

/* 요소를 나열합니다. */
struct list_elem *list_front (struct list *);
struct list_elem *list_back (struct list *);

/* 속성을 나열합니다. */
size_t list_size (struct list *);
bool list_empty (struct list *);

/* 여러 가지 잡다한. */
void list_reverse (struct list *);

/* 주어진 두 목록 요소 A와 B의 값을 비교합니다.
   보조 데이터 AUX.  
   A가 B보다 크거나 그러하다면 거짓입니다. */
typedef bool list_less_func (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux);

/* 순서가 지정된 요소가 있는 목록에 대한 작업입니다. */
void list_sort (struct list *,
                list_less_func *, void *aux);
void list_insert_ordered (struct list *, struct list_elem *,
                          list_less_func *, void *aux);
void list_unique (struct list *, struct list *duplicates,
                  list_less_func *, void *aux);

/* 최대 및 최소 */
struct list_elem *list_max (struct list *, list_less_func *, void *aux);
struct list_elem *list_min (struct list *, list_less_func *, void *aux);

#endif /* lib/커널/list.h */
