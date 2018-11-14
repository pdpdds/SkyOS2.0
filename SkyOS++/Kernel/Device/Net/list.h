/*
** Copyright 2004, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef __KERNEL_LIST_H
#define __KERNEL_LIST_H

/* A Linux-inspired circular doubly linked list */

struct list_node {
	struct list_node *prev;
	struct list_node *next;
};

static inline void list_initialize(struct list_node *list)
{
	list->prev = list->next = list;
}

static inline void list_clear_node(struct list_node *item)
{
	item->prev = item->next = 0;
}

static inline void list_add_head(struct list_node *list, struct list_node *item)
{
	item->next = list->next;
	item->prev = list;
	list->next->prev = item;
	list->next = item;
}

static inline void list_add_tail(struct list_node *list, struct list_node *item)
{
	item->prev = list->prev;
	item->next = list;
	list->prev->next = item;
	list->prev = item;
}

static inline void list_delete(struct list_node *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	item->prev = item->next = 0;
}

static inline struct list_node* list_remove_head(struct list_node *list)
{
	if(list->next != list) {
		struct list_node *item = list->next;
		list_delete(item);
		return item;
	} else {
		return NULL;
	}
}

#define list_remove_head_type(list, type, element) ({\
    struct list_node *__nod = list_remove_head(list);\
    type *__t;\
    if(__nod)\
        __t = containerof(__nod, type, element);\
    else\
        __t = (type *)0;\
    __t;\
})

static inline struct list_node* list_remove_tail(struct list_node *list)
{
	if(list->prev != list) {
		struct list_node *item = list->prev;
		list_delete(item);
		return item;
	} else {
		return NULL;
	}
}

#define list_remove_tail_type(list, type, element) ({\
    struct list_node *__nod = list_remove_tail(list);\
    type *__t;\
    if(__nod)\
        __t = containerof(__nod, type, element);\
    else\
        __t = (type *)0;\
    __t;\
})

static inline struct list_node* list_peek_head(struct list_node *list)
{
	if(list->next != list) {
		return list->next;
	} else {
		return NULL;
	}	
}

#define list_peek_head_type(list, type, element) ({\
    struct list_node *__nod = list_peek_head(list);\
    type *__t;\
    if(__nod)\
        __t = containerof(__nod, type, element);\
    else\
        __t = (type *)0;\
    __t;\
})

static inline struct list_node* list_peek_tail(struct list_node *list)
{
	if(list->prev != list) {
		return list->prev;
	} else {
		return NULL;
	}	
}

#define list_peek_tail_type(list, type, element) ({\
    struct list_node *__nod = list_peek_tail(list);\
    type *__t;\
    if(__nod)\
        __t = containerof(__nod, type, element);\
    else\
        __t = (type *)0;\
    __t;\
})

static inline struct list_node* list_next(struct list_node *list, struct list_node *item)
{
	if(item->next != list)
		return item->next;
	else
		return NULL;
}

#define list_next_type(list, item, type, element) ({\
    struct list_node *__nod = list_next(list, item);\
    type *__t;\
    if(__nod)\
        __t = containerof(__nod, type, element);\
    else\
        __t = (type *)0;\
    __t;\
})

static inline struct list_node* list_next_wrap(struct list_node *list, struct list_node *item)
{
	if(item->next != list)
		return item->next;
	else if(item->next->next != list)
		return item->next->next;
	else
		return NULL;
}

#define list_next_wrap_type(list, item, type, element) ({\
    struct list_node *__nod = list_next_wrap(list, item);\
    type *__t;\
    if(__nod)\
        __t = containerof(__nod, type, element);\
    else\
        __t = (type *)0;\
    __t;\
})

// iterates over the list, node should be struct list_node*
#define list_for_every(list, node) \
	for(node = (list)->next; node != (list); node = node->next)

// iterates over the list in a safe way for deletion of current node
// node and temp_node should be struct list_node*
#define list_for_every_safe(list, node, temp_node) \
	for(node = (list)->next, temp_node = (node)->next;\
	node != (list);\
	node = temp_node, temp_node = (node)->next)

// iterates over the list, entry should be the container structure type *
#define list_for_every_entry(list, entry, type, member) \
	for((entry) = containerof((list)->next, type, member);\
		&(entry)->member != (list);\
		(entry) = containerof((entry)->member.next, type, member))

// iterates over the list in a safe way for deletion of current node
// entry and temp_entry should be the container structure type *
#define list_for_every_entry_safe(list, entry, temp_entry, type, member) \
	for(entry = containerof((list)->next, type, member),\
		temp_entry = containerof((entry)->member.next, type, member);\
		&(entry)->member != (list);\
		entry = temp_entry, temp_entry = containerof((temp_entry)->member.next, type, member))

static inline bool list_is_empty(struct list_node *list)
{
	return (list->next == list) ? true : false;
}

#endif
