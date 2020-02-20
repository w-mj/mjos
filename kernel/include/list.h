#pragma once
#include <base.h>

typedef struct _ListEntry {
	struct _ListEntry *prev, *next;
} ListEntry;

#define list_entry(ptr, type, member) container_of(ptr, type, member)
#define foreach(c, list) \
	for(c = &list.next; c != &list; c = c->next)

#define LIST_INIT ((ListEntry){NULL, NULL})

static inline void list_init(ListEntry* head) {
	head->prev = head;
	head->next = head;
}
static inline void __list_add(ListEntry *n,
                              ListEntry *prev,
                              ListEntry *next) {
	next->prev = n;
	n->next = next;
	n->prev = prev;
	prev->next = n;
}

static inline void list_add(ListEntry *n, ListEntry *head) {
	__list_add(n, head, head->next);
}

static inline void list_add_tail(ListEntry *n, ListEntry *head) {
	__list_add(n, head->prev, head);
}

static inline void list_remove(ListEntry *n) {
	n->next->prev = n->prev;
	n->prev->next = n->next;
	n->prev = n;
	n->next = n;
}

static inline u8 list_empty(ListEntry *head) {
	return head->next == head;
}

// insert x before y
static inline void list_insert_before(ListEntry *x, ListEntry *y) {
	list_add_tail(x, y);
}


static inline void list_pop_head(ListEntry *head) {
	if (head->next != head)
		list_remove(head->next);
}
