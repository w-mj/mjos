#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <base.h>

typedef struct _ListEntry {
	struct _ListEntry *prev, *next;
} ListEntry;

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define foreach(c, list) \
	for(c = (list).next; c != &(list); c = c->next)

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


static inline ListEntry *list_pop_head(ListEntry *head) {
	if (list_empty(head))
		return NULL;
	ListEntry *to_return = head->next;
	list_remove(to_return);
	return to_return;
}

typedef bool (EquFunc)(ListEntry*, void*);
static inline ListEntry *list_find(ListEntry *head, void *target, EquFunc func) {
	ListEntry *c;
	foreach(c, *head) {
		if (func(c, target))
			return c;
	}
	return NULL;
}

static inline bool list_remove_item(ListEntry *head, void *target, EquFunc func) {
	ListEntry *e = list_find(head, target, func);
	if (e == NULL)
		return false;
	list_remove(e);
	return true;
}
#ifdef __cplusplus
}
#endif
