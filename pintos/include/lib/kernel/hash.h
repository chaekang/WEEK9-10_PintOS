#ifndef __LIB_KERNEL_HASH_H
#define __LIB_KERNEL_HASH_H

/* 해시 테이블.
 *
 * 이 데이터 구조는 탐욕스럽게 문서화되어 있는 투어입니다.
 * 프로젝트 3의 핀토스.
 *
 * 체인이 적용된 표준 해시 테이블입니다.  
 * 테이블의 요소에 대해 해시 함수를 계산합니다.
 * 요소의 데이터를 배열의 인덱스로 사용합니다.
 * 이중 연결 목록을 사용한 다음 목록을 선형적으로 검색합니다.
 *
 * 체인 목록은 동적 할당을 사용하지 않습니다.  
 * 잠재적으로 해시에 포함될 수 있는 구조에는
 * 기억에 남는 hash_elem 기초.  
 * 이 `struct hash_elem'입니다.  
 * 복원된 hash_elem에서 복원으로 다시 변환
 *이 포함되어 있습니다.  
 * 연결리스트 구현.  
 * 자세한 설명. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "list.h"

/* 해시 요소. */
struct hash_elem {
	struct list_elem list_elem;
};

/* 내 요소 HASH_ELEM에 대한 포인터를 포인터로 변환합니다.
 * HASH_ELEM이 내부에 내장된 구조입니다.  
 * 외부 구조 STRUCT의 이름과 기초 이름 MEMBER
 * 해시 요소.  
 * 예시 파일입니다. */
#define hash_entry(HASH_ELEM, STRUCT, MEMBER)                   \
	((STRUCT *) ((uint8_t *) &(HASH_ELEM)->list_elem        \
		- offsetof (STRUCT, MEMBER.list_elem)))

/* 주어진 조건에 따라 해시 요소 E의 해시 값을 계산하고 반환합니다.
 * 보조 데이터 AUX. */
typedef uint64_t hash_hash_func (const struct hash_elem *e, void *aux);

/* 주어진 두 해시 요소 A와 B의 값을 비교합니다.
 * 보조 데이터 AUX.  
 * A가 B보다 크거나 같으면 거짓입니다. */
typedef bool hash_less_func (const struct hash_elem *a,
		const struct hash_elem *b,
		void *aux);

/* 보조가 주어지면 해시 요소 E에 대해 일부 작업을 수행합니다.
 * 데이터 AUX. */
typedef void hash_action_func (struct hash_elem *e, void *aux);

/* 해시 테이블. */
struct hash {
	size_t elem_cnt;            /* 테이블의 요소 수입니다. */
	size_t bucket_cnt;          /* 버킷 수, 2의 거듭제곱. */
	struct list *buckets;       /* `bucket_cnt' 목록의 배열. */
	hash_hash_func *hash;       /* 해시 함수. */
	hash_less_func *less;       /* 비교 기능. */
	void *aux;                  /* 'hash' 및 'less'에 대한 보조 데이터입니다. */
};

/* 해시 테이블 반복자입니다. */
struct hash_iterator {
	struct hash *hash;          /* 해시 테이블. */
	struct list *bucket;        /* 현재 버킷. */
	struct hash_elem *elem;     /* 현재 버킷의 현재 해시 요소입니다. */
};

/* 기본 수명주기. */
bool hash_init (struct hash *, hash_hash_func *, hash_less_func *, void *aux);
void hash_clear (struct hash *, hash_action_func *);
void hash_destroy (struct hash *, hash_action_func *);

/* 검색, 삽입, 삭제. */
struct hash_elem *hash_insert (struct hash *, struct hash_elem *);
struct hash_elem *hash_replace (struct hash *, struct hash_elem *);
struct hash_elem *hash_find (struct hash *, struct hash_elem *);
struct hash_elem *hash_delete (struct hash *, struct hash_elem *);

/* 반복. */
void hash_apply (struct hash *, hash_action_func *);
void hash_first (struct hash_iterator *, struct hash *);
struct hash_elem *hash_next (struct hash_iterator *);
struct hash_elem *hash_cur (struct hash_iterator *);

/* 정보. */
size_t hash_size (struct hash *);
bool hash_empty (struct hash *);

/* 샘플 해시 함수. */
uint64_t hash_bytes (const void *, size_t);
uint64_t hash_string (const char *);
uint64_t hash_int (int);

#endif /* lib/커널/hash.h */
