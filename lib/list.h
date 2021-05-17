#ifndef LIBFLEX_LIST_H
#define LIBFLEX_LIST_H

#include <stdbool.h>

typedef struct list_struct {
	struct list_struct *next;
	struct list_struct *prev;
} list_t;

#define LIST_HEAD(name) list_t name = {&(name), &(name)}

#define CONTAINER_OF(ptr, type, member) \
	(type *)((char *)(ptr) - (char *)(&((type *)0)->member))

#define __CONTAINER_OF(ptr, sample, member) \
	CONTAINER_OF(ptr, typeof(*(sample)), member)

#define LIST_FOR_EACH_ENTRY(pos, head, member) \
	for (pos = __CONTAINER_OF((head)->next, pos, member); \
			&pos->member != (head); \
			pos = __CONTAINER_OF(pos->member.next, pos, member))

#define LIST_FOR_EACH_ENTRY_REVERSE(pos, head, member) \
	for (pos = __CONTAINER_OF((head)->prev, pos, member); \
			&pos->member != (head); \
			pos = __CONTAINER_OF(pos->member.prev, pos, member))

#define LIST_FOR_EACH_ENTRY_SAFE(pos, tmp, head, member) \
	for (pos = __CONTAINER_OF((head)->next, pos, member), \
			tmp = __CONTAINER_OF(pos->member.next, pos, member); \
			&pos->member != (head); \
			pos = tmp, \
			tmp = __CONTAINER_OF(pos->member.next, tmp, member))

static inline void INIT_LIST_HEAD(list_t *head)
{
	head->next = head->prev = head;
}

static inline bool list_empty(list_t *head)
{
	return head->next == head;
}

static inline void __list_add(list_t *entry, list_t *prev, list_t *next)
{
	next->prev = entry;
	entry->next = next;
	entry->prev = prev;
	prev->next = entry;
}

static inline void list_add(list_t *entry, list_t *head)
{
	__list_add(entry, head, head->next);
}

static inline void list_add_tail(list_t *entry, list_t *head)
{
	__list_add(entry, head->prev, head);
}

static inline void __list_del(list_t *prev, list_t *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(list_t *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_move_tail(list_t *entry, list_t *head)
{
	__list_del(entry->prev, entry->next);
	list_add_tail(entry, head);
}

static inline void list_merge(list_t *head, list_t *other)
{
	head->prev->next = other->next;
	other->next->prev = head->prev;
	other->prev->next = head;
	head->prev = other->prev;
}

#endif /* LIBFLEX_LIST_H */
