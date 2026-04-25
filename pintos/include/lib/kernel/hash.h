#ifndef __LIB_KERNEL_HASH_H
#define __LIB_KERNEL_HASH_H

/* 이 구간의 동작과 의도를 설명한다. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "list.h"

/* 이 구간의 동작과 의도를 설명한다. */
struct hash_elem {
	struct list_elem list_elem;
};

/* 이 구간의 동작과 의도를 설명한다. */
#define hash_entry(HASH_ELEM, STRUCT, MEMBER)                   \
	((STRUCT *) ((uint8_t *) &(HASH_ELEM)->list_elem        \
		- offsetof (STRUCT, MEMBER.list_elem)))

/* 이 구간의 동작과 의도를 설명한다. */
typedef uint64_t hash_hash_func (const struct hash_elem *e, void *aux);

/* 이 구간의 동작과 의도를 설명한다. */
typedef bool hash_less_func (const struct hash_elem *a,
		const struct hash_elem *b,
		void *aux);

/* 이 구간의 동작과 의도를 설명한다. */
typedef void hash_action_func (struct hash_elem *e, void *aux);

/* 이 구간의 동작과 의도를 설명한다. */
struct hash {
	size_t elem_cnt;            /* 이 구간의 동작과 의도를 설명한다. */
	size_t bucket_cnt;          /* 이 구간의 동작과 의도를 설명한다. */
	struct list *buckets;       /* 이 구간의 동작과 의도를 설명한다. */
	hash_hash_func *hash;       /* 이 구간의 동작과 의도를 설명한다. */
	hash_less_func *less;       /* 이 구간의 동작과 의도를 설명한다. */
	void *aux;                  /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
struct hash_iterator {
	struct hash *hash;          /* 이 구간의 동작과 의도를 설명한다. */
	struct list *bucket;        /* 이 구간의 동작과 의도를 설명한다. */
	struct hash_elem *elem;     /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
bool hash_init (struct hash *, hash_hash_func *, hash_less_func *, void *aux);
void hash_clear (struct hash *, hash_action_func *);
void hash_destroy (struct hash *, hash_action_func *);

/* 이 구간의 동작과 의도를 설명한다. */
struct hash_elem *hash_insert (struct hash *, struct hash_elem *);
struct hash_elem *hash_replace (struct hash *, struct hash_elem *);
struct hash_elem *hash_find (struct hash *, struct hash_elem *);
struct hash_elem *hash_delete (struct hash *, struct hash_elem *);

/* 이 구간의 동작과 의도를 설명한다. */
void hash_apply (struct hash *, hash_action_func *);
void hash_first (struct hash_iterator *, struct hash *);
struct hash_elem *hash_next (struct hash_iterator *);
struct hash_elem *hash_cur (struct hash_iterator *);

/* 이 구간의 동작과 의도를 설명한다. */
size_t hash_size (struct hash *);
bool hash_empty (struct hash *);

/* 이 구간의 동작과 의도를 설명한다. */
uint64_t hash_bytes (const void *, size_t);
uint64_t hash_string (const char *);
uint64_t hash_int (int);

#endif /* 이 구간의 동작과 의도를 설명한다. */
