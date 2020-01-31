#pragma once
#include <base.h>

typedef struct _ListEntry {
	struct _ListEntry *prev, *next;
} ListEntry;


#define init_list(x) do {\
	x.prev = &x; \
	x.next = &x; \
} while(0)


#define list_entry(ptr, type, member) container_of(ptr, type, member)
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

static inline void list_remove(ListEntry *n) {
	n->next->prev = n->prev;
	n->prev->next = n->next;
	n->prev = n;
	n->next = n;
}
