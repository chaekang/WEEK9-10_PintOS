#ifndef __LIB_KERNEL_LIST_H
#define __LIB_KERNEL_LIST_H

/* 이 구간의 동작과 의도를 설명한다. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 이 구간의 동작과 의도를 설명한다. */
struct list_elem {
	struct list_elem *prev;     /* 이 구간의 동작과 의도를 설명한다. */
	struct list_elem *next;     /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
struct list {
	struct list_elem head;      /* 이 구간의 동작과 의도를 설명한다. */
	struct list_elem tail;      /* 이 구간의 동작과 의도를 설명한다. */
};

/* 이 구간의 동작과 의도를 설명한다. */
#define list_entry(LIST_ELEM, STRUCT, MEMBER)           \
	((STRUCT *) ((uint8_t *) &(LIST_ELEM)->next     \
		- offsetof (STRUCT, MEMBER.next)))

void list_init (struct list *);

/* 이 구간의 동작과 의도를 설명한다. */
struct list_elem *list_begin (struct list *);
struct list_elem *list_next (struct list_elem *);
struct list_elem *list_end (struct list *);

struct list_elem *list_rbegin (struct list *);
struct list_elem *list_prev (struct list_elem *);
struct list_elem *list_rend (struct list *);

struct list_elem *list_head (struct list *);
struct list_elem *list_tail (struct list *);

/* 이 구간의 동작과 의도를 설명한다. */
void list_insert (struct list_elem *, struct list_elem *);
void list_splice (struct list_elem *before,
		struct list_elem *first, struct list_elem *last);
void list_push_front (struct list *, struct list_elem *);
void list_push_back (struct list *, struct list_elem *);

/* 이 구간의 동작과 의도를 설명한다. */
struct list_elem *list_remove (struct list_elem *);
struct list_elem *list_pop_front (struct list *);
struct list_elem *list_pop_back (struct list *);

/* 이 구간의 동작과 의도를 설명한다. */
struct list_elem *list_front (struct list *);
struct list_elem *list_back (struct list *);

/* 이 구간의 동작과 의도를 설명한다. */
size_t list_size (struct list *);
bool list_empty (struct list *);

/* 이 구간의 동작과 의도를 설명한다. */
void list_reverse (struct list *);

/* 이 구간의 동작과 의도를 설명한다. */
typedef bool list_less_func (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux);

/* 이 구간의 동작과 의도를 설명한다. */
void list_sort (struct list *,
                list_less_func *, void *aux);
void list_insert_ordered (struct list *, struct list_elem *,
                          list_less_func *, void *aux);
void list_unique (struct list *, struct list *duplicates,
                  list_less_func *, void *aux);

/* 이 구간의 동작과 의도를 설명한다. */
struct list_elem *list_max (struct list *, list_less_func *, void *aux);
struct list_elem *list_min (struct list *, list_less_func *, void *aux);

#endif /* 이 구간의 동작과 의도를 설명한다. */
