/* 해시 테이블.

   이 데이터 구조는 탐욕스럽게 문서화되어 있는 둘러보기입니다.
   프로젝트 3의 핀토스.

   기본 정보는 hash.h를 참조하세요. */

#include "hash.h"
#include "../debug.h"
#include "threads/malloc.h"

#define list_elem_to_hash_elem(LIST_ELEM)                       \
	list_entry(LIST_ELEM, struct hash_elem, list_elem)

static struct list *find_bucket (struct hash *, struct hash_elem *);
static struct hash_elem *find_elem (struct hash *, struct list *,
		struct hash_elem *);
static void insert_elem (struct hash *, struct list *, struct hash_elem *);
static void remove_elem (struct hash *, struct hash_elem *);
static void rehash (struct hash *);

/* HASH를 사용하여 언제 가치를 꺼내기 위해 따뜻한 테이블 H를 가지고 뛰어오기
   데이터를 보조하는 AUX를 다루면 LESS를 사용하여서 요소를 비교합니다. */
bool
hash_init (struct hash *h,
		hash_hash_func *hash, hash_less_func *less, void *aux) {
	h->elem_cnt = 0;
	h->bucket_cnt = 4;
	h->buckets = malloc (sizeof *h->buckets * h->bucket_cnt);
	h->hash = hash;
	h->less = less;
	h->aux = aux;

	if (h->buckets != NULL) {
		hash_clear (h, NULL);
		return true;
	} else
		return false;
}

/* H에서 모든 요소를 ​​제거합니다.

   DESTRUCTOR가 null이 아닌 경우 요소에 호출되었습니다.
   해시에서.  
   해시 요소가 사용하는 메모리입니다.  
   hash_clear()가 실행되는 동안 테이블 H 중 하나를 사용하여
   함수 hash_clear(), hash_destroy(), hash_insert(),
   hash_replace() 또는 hash_delete()는 정의되지 않은 동작을 생성합니다.
   DESTRUCTOR 또는 다른 장소에서 수행되었는지 여부. */
void
hash_clear (struct hash *h, hash_action_func *destructor) {
	size_t i;

	for (i = 0; i < h->bucket_cnt; i++) {
		struct list *bucket = &h->buckets[i];

		if (destructor != NULL)
			while (!list_empty (bucket)) {
				struct list_elem *list_elem = list_pop_front (bucket);
				struct hash_elem *hash_elem = list_elem_to_hash_elem (list_elem);
				destructor (hash_elem, h->aux);
			}

		list_init (bucket);
	}

	h->elem_cnt = 0;
}

/* 해시 테이블 H를 파괴합니다.

   DESTRUCTOR가 null이 아닌 경우 항목에 먼저 호출됩니다.
   해시의 요소입니다.  
   해시 요소에 사용된 메모리 할당을 해제합니다.  
   hash_clear()가 실행되는 동안 내내 테이블 H를 수정합니다.
   hash_clear(), hash_destroy(),
   hash_insert(), hash_replace() 또는 hash_delete()는 다음을 생성합니다.
   소멸자 또는
   다른 곳. */
void
hash_destroy (struct hash *h, hash_action_func *destructor) {
	if (destructor != NULL)
		hash_clear (h, destructor);
	free (h->buckets);
}

/* 해시 테이블 H에 NEW를 삽입하고 다음과 같은 경우 널 포인터를 반환합니다.
   테이블에 동일한 요소가 이미 없습니다.
   동일한 요소가 이미 테이블에 있으면 해당 요소를 반환합니다.
   NEW를 삽입하지 않고. */
struct hash_elem *
hash_insert (struct hash *h, struct hash_elem *new) {
	struct list *bucket = find_bucket (h, new);
	struct hash_elem *old = find_elem (h, bucket, new);

	if (old == NULL)
		insert_elem (h, bucket, new);

	rehash (h);

	return old;
}

/* 해시 테이블 H에 NEW를 삽입하여 동일한 요소를 대체합니다.
   이미 반환된 테이블에 있습니다. */
struct hash_elem *
hash_replace (struct hash *h, struct hash_elem *new) {
	struct list *bucket = find_bucket (h, new);
	struct hash_elem *old = find_elem (h, bucket, new);

	if (old != NULL)
		remove_elem (h, old);
	insert_elem (h, bucket, new);

	rehash (h);

	return old;
}

/* 해시 테이블 H에서 E와 동일한 요소를 찾아서 반환합니다.
   테이블에 동일한 요소가 없으면 null 포인터입니다. */
struct hash_elem *
hash_find (struct hash *h, struct hash_elem *e) {
	return find_elem (h, find_bucket (h, e), e);
}

/* 해시에서 E와 동일한 요소를 찾아 제거하고 반환합니다.
   테이블 H. 동일한 요소가 없으면 널 포인터를 반환합니다.
   테이블에.

   해시 테이블의 요소가 동적으로 할당되는 경우
   또는 자신의 리소스인 경우 호출자의 리소스입니다.
   할당을 취소할 책임이 있습니다. */
struct hash_elem *
hash_delete (struct hash *h, struct hash_elem *e) {
	struct hash_elem *found = find_elem (h, find_bucket (h, e), e);
	if (found != NULL) {
		remove_elem (h, found);
		rehash (h);
	}
	return found;
}

/* 외부 테이블 H의 각 요소에 대해 처리 ACTION을 호출합니다.
   주문하다.
   hash_apply()가 실행되는 동안 테이블 H 수정
   hash_clear(), hash_destroy(),
   hash_insert(), hash_replace() 또는 hash_delete()는 다음을 생성합니다.
   ACTION에서 다른 곳에서 정의되지 않은 동작입니다. */
void
hash_apply (struct hash *h, hash_action_func *action) {
	size_t i;

	ASSERT (action != NULL);

	for (i = 0; i < h->bucket_cnt; i++) {
		struct list *bucket = &h->buckets[i];
		struct list_elem *elem, *next;

		for (elem = list_begin (bucket); elem != list_end (bucket); elem = next) {
			next = list_next (elem);
			action (list_elem_to_hash_elem (elem), h->aux);
		}
	}
}

/* 해시 테이블 H를 반복하기 위해 I를 초기화합니다.

   반복 관용어:

   구조체 hash_iterator i;

   hash_first (&i, h);
   동안(hash_next(&i))
   {
   struct foo *f = hash_entry (hash_cur (&i), struct foo, elem);
   ...f로 뭔가를 해보세요...
   }

   다음 중 하나를 사용하여 반복 중에 해시 테이블 H 수정
   함수 hash_clear(), hash_destroy(), hash_insert(),
   hash_replace() 또는 hash_delete()는 모든 것을 초기화합니다.
   반복자. */
void
hash_first (struct hash_iterator *i, struct hash *h) {
	ASSERT (i != NULL);
	ASSERT (h != NULL);

	i->hash = h;
	i->bucket = i->hash->buckets;
	i->elem = list_elem_to_hash_elem (list_head (i->bucket));
}

/* I를 해시 테이블의 다음 요소로 이동하고 반환합니다.
   그것.  
   임의의 순서로 반환됩니다.

   다음 중 하나를 사용하여 반복 중에 해시 테이블 H 수정
   함수 hash_clear(), hash_destroy(), hash_insert(),
   hash_replace() 또는 hash_delete()는 모든 것을 초기화합니다.
   반복자. */
struct hash_elem *
hash_next (struct hash_iterator *i) {
	ASSERT (i != NULL);

	i->elem = list_elem_to_hash_elem (list_next (&i->elem->list_elem));
	while (i->elem == list_elem_to_hash_elem (list_end (i->bucket))) {
		if (++i->bucket >= i->hash->buckets + i->hash->bucket_cnt) {
			i->elem = NULL;
			break;
		}
		i->elem = list_elem_to_hash_elem (list_begin (i->bucket));
	}

	return i->elem;
}

/* 해시 테이블 반복에서 현재 요소를 반환합니다.
   테이블 끝에 널 포인터가 있습니다.  
   hash_first() 호출 후, hash_next() 호출 전. */
struct hash_elem *
hash_cur (struct hash_iterator *i) {
	return i->elem;
}

/* H의 요소 수를 반환합니다. */
size_t
hash_size (struct hash *h) {
	return h->elem_cnt;
}

/* H에 요소가 없으면 true를 반환하고 그렇지 않으면 false를 반환합니다. */
bool
hash_empty (struct hash *h) {
	return h->elem_cnt == 0;
}

/* Fowler-Noll-Vo 굴절(32비트 단어 크기용) */
#define FNV_64_PRIME 0x00000100000001B3UL
#define FNV_64_BASIS 0xcbf29ce484222325UL

/* BUF에서 SIZE를 반환합니다. */
uint64_t
hash_bytes (const void *buf_, size_t size) {
	/* Fowler-Noll-Vo 32비트 타이밍(업무용) */
	const unsigned char *buf = buf_;
	uint64_t hash;

	ASSERT (buf != NULL);

	hash = FNV_64_BASIS;
	while (size-- > 0)
		hash = (hash * FNV_64_PRIME) ^ *buf++;

	return hash;
}

/* 문자열 S의 해시를 반환합니다. */
uint64_t
hash_string (const char *s_) {
	const unsigned char *s = (const unsigned char *) s_;
	uint64_t hash;

	ASSERT (s != NULL);

	hash = FNV_64_BASIS;
	while (*s != '\0')
		hash = (hash * FNV_64_PRIME) ^ *s++;

	return hash;
}

/* 정수 I의 해시를 반환합니다. */
uint64_t
hash_int (int i) {
	return hash_bytes (&i, sizeof i);
}

/* E가 속한 H의 버킷을 반환합니다. */
static struct list *
find_bucket (struct hash *h, struct hash_elem *e) {
	size_t bucket_idx = h->hash (e, h->aux) & (h->bucket_cnt - 1);
	return &h->buckets[bucket_idx];
}

/* H의 BUCKET에서 E와 동등한 요소를 검색합니다.
   발견하면 null 포인터이고, 그렇지 않으면 null 포인터입니다. */
static struct hash_elem *
find_elem (struct hash *h, struct list *bucket, struct hash_elem *e) {
	struct list_elem *i;

	for (i = list_begin (bucket); i != list_end (bucket); i = list_next (i)) {
		struct hash_elem *hi = list_elem_to_hash_elem (i);
		if (!h->less (hi, e, h->aux) && !h->less (e, hi, h->aux))
			return hi;
	}
	return NULL;
}

/* 최하위 비트가 1로 설정된 X를 반환합니다. */
static inline size_t
turn_off_least_1bit (size_t x) {
	return x & (x - 1);
}

/* X가 2의 제곱이면 true를 반환하고, 그렇지 않으면 false를 반환합니다. */
static inline size_t
is_power_of_2 (size_t x) {
	return x != 0 && turn_off_least_1bit (x) == 0;
}

/* 버킷당 요소 비율. */
#define MIN_ELEMS_PER_BUCKET  1 /* 요소/버킷 < 1: 버킷 수를 줄입니다. */
#define BEST_ELEMS_PER_BUCKET 2 /* 이상적인 요소/버킷. */
#define MAX_ELEMS_PER_BUCKET  4 /* 요소/버킷 > 4: 버킷 수를 늘립니다. */

/* 해시 테이블 H의 버킷 수를 일치하도록 변경합니다.
   이상적인.  
   하지만 이는 해시 액세스의 효율성을 떨어뜨릴 뿐입니다.
   우리는 아직 계속할 수 있습니다. */
static void
rehash (struct hash *h) {
	size_t old_bucket_cnt, new_bucket_cnt;
	struct list *new_buckets, *old_buckets;
	size_t i;

	ASSERT (h != NULL);

	/* 나중에 사용할 수 있도록 이전 버킷 정보를 저장하세요. */
	old_buckets = h->buckets;
	old_bucket_cnt = h->bucket_cnt;

	/* 지금 사용할 버킷 수를 계산하십시오.
	   우리는 BEST_ELEMS_PER_BUCKET마다 하나의 역할을 원합니다.
	   최소한 4개의 버킷이 있어야 하며,
	   버킷은 2의 거듭제곱이어야 합니다. */
	new_bucket_cnt = h->elem_cnt / BEST_ELEMS_PER_BUCKET;
	if (new_bucket_cnt < 4)
		new_bucket_cnt = 4;
	while (!is_power_of_2 (new_bucket_cnt))
		new_bucket_cnt = turn_off_least_1bit (new_bucket_cnt);

	/* 버킷 수가 변경되지 않으면 아무것도 하지 마세요. */
	if (new_bucket_cnt == old_bucket_cnt)
		return;

	/* 새 버킷을 할당하고 비어 있는 상태로 초기화합니다. */
	new_buckets = malloc (sizeof *new_buckets * new_bucket_cnt);
	if (new_buckets == NULL) {
		/* 할당에 실패했습니다.  
		   효율성이 떨어집니다.  
		   오류가 될 이유가 없습니다. */
		return;
	}
	for (i = 0; i < new_bucket_cnt; i++)
		list_init (&new_buckets[i]);

	/* 새로운 버킷 정보를 설치합니다. */
	h->buckets = new_buckets;
	h->bucket_cnt = new_bucket_cnt;

	/* 각각의 이전 요소를 적절한 새 버킷으로 이동합니다. */
	for (i = 0; i < old_bucket_cnt; i++) {
		struct list *old_bucket;
		struct list_elem *elem, *next;

		old_bucket = &old_buckets[i];
		for (elem = list_begin (old_bucket);
				elem != list_end (old_bucket); elem = next) {
			struct list *new_bucket
				= find_bucket (h, list_elem_to_hash_elem (elem));
			next = list_next (elem);
			list_remove (elem);
			list_push_front (new_bucket, elem);
		}
	}

	free (old_buckets);
}

/* E를 BUCKET(해시테이블 H)에 삽입합니다. */
static void
insert_elem (struct hash *h, struct list *bucket, struct hash_elem *e) {
	h->elem_cnt++;
	list_push_front (bucket, &e->list_elem);
}

/* 해시 테이블 H에서 E를 제거합니다. */
static void
remove_elem (struct hash *h, struct hash_elem *e) {
	h->elem_cnt--;
	list_remove (&e->list_elem);
}

