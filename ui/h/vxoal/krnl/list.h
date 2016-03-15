/* list.h - list functionality header file*/

/*
 * Copyright (c) 1999-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
21jul15,yat  Clean up vxoal (US63380)
01jun15,rpc  porting DRM/i915 3.18 (US59495)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux list operations.
hash operations too.

NOMANUAL
*/

#ifndef _VXOAL_LIST_H
#define _VXOAL_LIST_H

#include <vxoal/krnl/types.h> /* for ARRAY_SIZE */
#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/ilog2.h> /* for ilog2 */

/* Simple doubly linked list implementation. */
struct list_head
    {
    struct list_head *next;
    struct list_head *prev;
    };

#define LIST_HEAD_INIT(name)    { &(name), &(name) }
#define LIST_HEAD(name)         struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr)                                                    \
    ({                                                                         \
    (ptr)->next = (ptr);                                                       \
    (ptr)->prev = (ptr);                                                       \
    })

#define list_entry(ptr, type, member)                                          \
    container_of(ptr, type, member)

#define list_last_entry(ptr, type, member)                                     \
    list_entry((ptr)->prev, type, member)

#define list_next_entry(pos, member)                                           \
    list_entry((pos)->member.next, __typeof__(*pos), member)

#define list_first_entry(ptr, type, member)                                    \
    list_entry((ptr)->next, type, member)

#define list_first_entry_or_null(ptr, type, member)                            \
    (!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

#define list_for_each(pos, head)                                               \
    for (pos = (head)->next; prefetch(pos->next), pos != (head);               \
        pos = pos->next)

#define list_for_each_prev(pos, head)                                          \
    for (pos = (head)->prev; prefetch(pos->prev), pos != (head);               \
        pos = pos->prev)

#define list_for_each_safe(pos, n, head)                                       \
    for (pos = (head)->next, n = pos->next; prefetch(pos->next), pos != (head);\
        pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, head)                                  \
    for (pos = (head)->prev, n = pos->prev; prefetch(pos->prev), pos != (head);\
        pos = n, n = pos->prev)

#define list_for_each_entry(pos, head, member)                                 \
    for (pos = list_entry((head)->next, __typeof__(*pos), member);             \
        prefetch(pos->member.next), &pos->member != (head);                    \
        pos = list_entry(pos->member.next, __typeof__(*pos), member))

#define list_for_each_entry_reverse(pos, head, member)                         \
    for (pos = list_entry((head)->prev, __typeof__(*pos), member);             \
        prefetch(pos->member.prev), &pos->member != (head);                    \
        pos = list_entry(pos->member.prev, __typeof__(*pos), member))

#define list_for_each_entry_continue(pos, head, member)                        \
    for (pos = list_entry(pos->member.next, __typeof__(*pos), member);         \
        prefetch(pos->member.next), &pos->member != (head);                    \
        pos = list_entry(pos->member.next, __typeof__(*pos), member))

#define list_for_each_entry_from(pos, head, member)                            \
    for (; prefetch(pos->member.next), &pos->member != (head);                 \
        pos = list_entry(pos->member.next, __typeof__(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)                         \
    for (pos = list_entry((head)->next, __typeof__(*pos), member),             \
        n = list_entry(pos->member.next, __typeof__(*pos), member);            \
        &pos->member != (head);                                                \
        pos = n, n = list_entry(n->member.next, __typeof__(*n), member))

#define list_for_each_entry_safe_reverse(pos, n, head, member)                 \
    for (pos = list_entry((head)->prev, __typeof__(*pos), member),             \
        n = list_entry(pos->member.prev, __typeof__(*pos), member);            \
        &pos->member != (head);                                                \
        pos = n, n = list_entry(n->member.prev, __typeof__(*n), member))

#define list_for_each_entry_safe_continue(pos, n, head, member)                \
    for (pos = list_entry(pos->member.next, __typeof__(*pos), member),         \
        n = list_entry(pos->member.next, __typeof__(*pos), member);            \
        &pos->member != (head);                                                \
        pos = n, n = list_entry(n->member.next, __typeof__(*n), member))

#define list_for_each_entry_safe_from(pos, n, head, member)                    \
    for (n = list_entry(pos->member.next, __typeof__(*pos), member);           \
        &pos->member != (head);                                                \
        pos = n, n = list_entry(n->member.next, __typeof__(*n), member))

/*******************************************************************************
*
* __list_add - Insert a new entry between two known consecutive entries.
*
* This is only for internal list manipulation where we know the prev/next
* entries already.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void __list_add
    (
    struct list_head *new,
    struct list_head *prev,
    struct list_head *next
    )
    {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
    }

/*******************************************************************************
*
* list_add - add a new entry
*
* Insert a new entry after the specified head.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_add
    (
    struct list_head *new,  /* new entry to be added */
    struct list_head *head  /* list head to add it after */
    )
    {
    __list_add(new, head, head->next);
    }

/*******************************************************************************
*
* list_add_tail - add a new entry
*
* Insert a new entry before the specified head.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_add_tail
    (
    struct list_head *new,  /* new entry to be added */
    struct list_head *head  /* list head to add it before */
    )
    {
    __list_add(new, head->prev, head);
    }

/*******************************************************************************
*
* __list_del - deletes entry from list.
*
* Delete a list entry by making the prev/next entries point to each other.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void __list_del
    (
    struct list_head *prev,
    struct list_head *next
    )
    {
    next->prev = prev;
    prev->next = next;
    }

/*******************************************************************************
*
* list_del - deletes entry from list.
*
* Note: list_empty() on entry does not return true after this, the entry is in
* an undefined state.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_del
    (
    struct list_head *entry     /* the element to delete from the list */
    )
    {
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
    }

/*******************************************************************************
*
* list_replace - replace old entry by new one
*
* If old was empty, it will be overwritten.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_replace
    (
    struct list_head *old,  /* the element to be replaced */
    struct list_head *new   /* the new element to insert */
    )
    {
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
    }

/*******************************************************************************
*
* list_replace_init - replaces entry from list and reinitialize it.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_replace_init
    (
    struct list_head *old,
    struct list_head *new
    )
    {
    list_replace(old, new);
    INIT_LIST_HEAD(old);
    }

/*******************************************************************************
*
* list_del_init - deletes entry from list and reinitialize it.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_del_init
    (
    struct list_head *entry     /* the element to delete from the list */
    )
    {
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
    }

/*******************************************************************************
*
* list_move - delete from one list and add as another's head
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_move
    (
    struct list_head *list,     /* the entry to move */
    struct list_head *head      /* the head that will precede our entry */
    )
    {
    __list_del(list->prev, list->next);
    list_add(list, head);
    }

/*******************************************************************************
*
* list_move_tail - delete from one list and add as another's tail
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_move_tail
    (
    struct list_head *list,     /* the entry to move */
    struct list_head *head      /* the head that will follow our entry */
    )
    {
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
    }

/*******************************************************************************
*
* list_is_last - tests whether list is the last entry in head
*
* RETURNS: 1 if the list is the last entry, or 0 otherwise
*
* SEE ALSO:
*/
static inline int list_is_last
    (
    const struct list_head *list,   /* the entry to test */
    const struct list_head *head    /* the head of the list */
    )
    {
    return list->next == head;
    }

/*******************************************************************************
*
* list_empty - tests whether a list is empty
*
* RETURNS: 1 if the list is empty, or 0 otherwise
*
* SEE ALSO:
*/
static inline int list_empty
    (
    const struct list_head *head    /* the list to test */
    )
    {
    return head->next == head;
    }

/*******************************************************************************
*
* list_empty_careful - tests whether a list is empty and not being modified
*
* Tests whether a list is empty and checks that no other CPU might be in the
* process of modifying either member (next or prev)
*
* NOTE: using list_empty_careful() without synchronization can only be safe if
* the only activity that can happen to the list entry is list_del_init().
* Eg. it cannot be used if another CPU could re-list_add() it.
*
* RETURNS: 1 if the list is empty, or 0 otherwise
*
* SEE ALSO:
*/
static inline int list_empty_careful
    (
    const struct list_head *head    /* the list to test */
    )
    {
    struct list_head *next = head->next;
    return (next == head) && (next == head->prev);
    }

/*******************************************************************************
*
* list_rotate_left - rotate the list to the left
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_rotate_left
    (
    struct list_head *head      /* the head of the list */
    )
    {
    struct list_head *first;

    if (!list_empty(head))
        {
        first = head->next;
        list_move_tail(first, head);
        }
    }

/*******************************************************************************
*
* list_is_singular - tests whether a list has just one entry
*
* RETURNS: 1 if the list is singular, or 0 otherwise
*
* SEE ALSO:
*/
static inline int list_is_singular
    (
    const struct list_head *head    /* the list to test */
    )
    {
    return !list_empty(head) && (head->next == head->prev);
    }

/*******************************************************************************
*
* __list_cut_position -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void __list_cut_position
    (
    struct list_head *list,
    struct list_head *head,
    struct list_head *entry
    )
    {
    struct list_head *new_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = new_first;
    new_first->prev = head;
    }

/*******************************************************************************
*
* list_cut_position - cut a list into two
*
* This helper moves the initial part of head, up to and including entry, from
* head to list. You should pass on entry an element you know is on head. list
* should be an empty list or a list you do not care about losing its data.
*
* entry and head can be the same, however if so, the list won't be cut.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_cut_position
    (
    struct list_head *list,     /* a new list to add all removed entries */
    struct list_head *head,     /* a list with entries */
    struct list_head *entry     /* an entry within head */
    )
    {
    if (list_empty(head))
        {
        return;
        }
    if (list_is_singular(head) && (head->next != entry && head != entry))
        {
        return;
        }
    if (entry == head)
        {
        INIT_LIST_HEAD(list);
        }
    else
        {
        __list_cut_position(list, head, entry);
        }
    }

/*******************************************************************************
*
* __list_splice -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void __list_splice
    (
    const struct list_head *list,
    struct list_head *prev,
    struct list_head *next
    )
    {
    struct list_head *first = list->next;
    struct list_head *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
    }

/*******************************************************************************
*
* list_splice - join two lists
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_splice
    (
    const struct list_head *list,   /* the new list to add */
    struct list_head *head          /* the place to add it in the first list */
    )
    {
    if (!list_empty(list))
        {
        __list_splice(list, head, head->next);
        }
    }

/*******************************************************************************
*
* list_splice_tail - join two lists
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_splice_tail
    (
    struct list_head *list,     /* the new list to add */
    struct list_head *head      /* the place to add it in the first list */
    )
    {
    if (!list_empty(list))
        {
        __list_splice(list, head->prev, head);
        }
    }

/*******************************************************************************
*
* list_splice_init - join two lists and reinitialise the emptied list
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_splice_init
    (
    struct list_head *list,     /* the new list to add */
    struct list_head *head      /* the place to add it in the first list */
    )
    {
    if (!list_empty(list))
        {
        __list_splice(list, head, head->next);
        INIT_LIST_HEAD(list);
        }
    }

/*******************************************************************************
*
* list_splice_tail_init - join two lists and reinitialise the emptied list
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void list_splice_tail_init
    (
    struct list_head *list,     /* the new list to add */
    struct list_head *head      /* the place to add it in the first list */
    )
    {
    if (!list_empty(list))
        {
        __list_splice(list, head->prev, head);
        INIT_LIST_HEAD(list);
        }
    }

extern void list_sort
    (
    void *priv,
    struct list_head *head,
    int (*cmp)(void *priv, struct list_head *a, struct list_head *b)
    );


/* Double linked lists with a single pointer list head. */
struct hlist_node
    {
    struct hlist_node *next;
    struct hlist_node **pprev;
    };

struct hlist_head
    {
    struct hlist_node *first;
    };

#define HLIST_HEAD_INIT         { .first = NULL }
#define HLIST_HEAD(name)        struct hlist_head name = HLIST_HEAD_INIT

#define INIT_HLIST_HEAD(ptr)    ((ptr)->first = NULL)

#define INIT_HLIST_NODE(ptr)                                                   \
    ({                                                                         \
    (ptr)->next = NULL;                                                        \
    (ptr)->pprev = NULL;                                                       \
    })

#define hlist_entry(ptr, type, member)                                         \
    container_of(ptr, type, member)

#define hlist_for_each(pos, head)                                              \
    for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; });            \
        pos = pos->next)

#define hlist_for_each_safe(pos, n, head)                                      \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; });                  \
        pos = n)

#if 0
#define hlist_for_each_entry(tpos, pos, head, member)                          \
    for (pos = (head)->first;                                                  \
        pos && ({ prefetch(pos->next); 1;}) &&                                 \
        ({ tpos = hlist_entry(pos, __typeof__(*tpos), member); 1;});           \
        pos = pos->next)
#else
#define hlist_for_each_entry(pos, head, member)                                \
    for (pos = (void*)(head)->first;                                           \
        pos && ({ prefetch(((struct hlist_node*)pos)->next);                   \
        pos = hlist_entry((void*)pos, __typeof__(*pos), member); 1; });        \
        pos = (void*)(pos)->member.next)
#endif

#define hlist_for_each_entry_continue(tpos, pos, member)                       \
    for (pos = (pos)->next;                                                    \
        pos && ({ prefetch(pos->next); 1;}) &&                                 \
        ({ tpos = hlist_entry(pos, __typeof__(*tpos), member); 1;});           \
        pos = pos->next)

#define hlist_for_each_entry_from(tpos, pos, member)                           \
    for (; pos && ({ prefetch(pos->next); 1;}) &&                              \
        ({ tpos = hlist_entry(pos, __typeof__(*tpos), member); 1;});           \
        pos = pos->next)

#if 0
#define hlist_for_each_entry_safe(tpos, pos, n, head, member)                  \
    for (pos = (head)->first;                                                  \
        pos && ({ n = pos->next; 1; }) &&                                      \
        ({ tpos = hlist_entry(pos, __typeof__(*tpos), member); 1;});           \
        pos = n)
#else
#define hlist_for_each_entry_safe(pos, n, head, member)                        \
    for (pos = (void*)(head)->first;                                           \
        pos && ({ n = (((struct hlist_node*)pos)->next);                       \
        pos = hlist_entry((void*)pos, __typeof__(*pos), member); 1; });        \
        pos = (void*)n)
#endif

#if 0
#define hlist_for_each_entry_rcu(tpos, pos, head, member)                      \
    for (pos = (head)->first;                                                  \
        pos && ({ prefetch(pos->next); 1;}) &&                                 \
        ({ tpos = hlist_entry(pos, __typeof__(*tpos), member); 1;});           \
        pos = pos->next, ({ smp_read_barrier_depends(); 0; }) )
#else
#define hlist_for_each_entry_rcu(pos, head, member)                            \
    for (pos = (void*)(head)->first;                                           \
        pos && ({ prefetch(((struct hlist_node*)pos)->next);                   \
        pos = hlist_entry((void*)pos, __typeof__(*pos), member); 1; });        \
        pos = (void*)(pos)->member.next)
#endif

static inline int hlist_unhashed
    (
    const struct hlist_node *h
    )
    {
    return !h->pprev;
    }

static inline int hlist_empty
    (
    const struct hlist_head *h
    )
    {
    return !h->first;
    }

static inline void __hlist_del
    (
    struct hlist_node *n
    )
    {
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
        {
        next->pprev = pprev;
        }
    }

static inline void hlist_del
    (
    struct hlist_node *n
    )
    {
    __hlist_del(n);
    n->next = NULL;
    n->pprev = NULL;
    }

static inline void hlist_del_init
    (
    struct hlist_node *n
    )
    {
    if (!hlist_unhashed(n))
        {
        __hlist_del(n);
        INIT_HLIST_NODE(n);
        }
    }

#define hlist_del_init_rcu hlist_del_init

static inline void hlist_add_head
    (
    struct hlist_node *n,
    struct hlist_head *h
    )
    {
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        {
        first->pprev = &n->next;
        }
    h->first = n;
    n->pprev = &h->first;
    }

#define hlist_add_head_rcu hlist_add_head

static inline void hlist_add_behind
    (
    struct hlist_node *n,
    struct hlist_node *prev
    )
    {
    n->next = prev->next;
    prev->next = n;
    n->pprev = &prev->next;
    if (n->next)
        {
        n->next->pprev  = &n->next;
        }
    }

#define hlist_add_behind_rcu hlist_add_behind


/* Hash table */
#define DECLARE_HASHTABLE(ht, bits) struct hlist_head ht[1 << (bits)]

#define HASH_SIZE(ht)           (ARRAY_SIZE(ht))
#define HASH_BITS(ht)           ilog2(HASH_SIZE(ht))

#if 0
#define DEBUG_HASH pr_err
#else
#define DEBUG_HASH(...)
#endif

#define hash_init(ht)                                                          \
    ({                                                                         \
    unsigned int i;                                                            \
    for (i = 0; i < HASH_SIZE(ht); i++)                                        \
        {                                                                      \
        INIT_HLIST_HEAD(&ht[i]);                                               \
        }                                                                      \
    DEBUG_HASH("ht:%d %d\n", HASH_SIZE(ht), HASH_BITS(ht));                    \
    })

/* Since this is used only to add max 31 cmds into the cmd_hash with 512 lists
   a simple shift is sufficient without collision */
#define hash_func(key, bits)                                                   \
    ({                                                                         \
    unsigned int val = (key);                                                  \
    DEBUG_HASH("key:%x bits:%d val:%d\n", key, bits, (val >> (32 - (bits))));  \
    (val >> (32 - (bits)));                                                    \
    })

/* Currently this is defined for drm_hashtab but not executed */
#define hash_long(key, bits)                                                   \
    ({                                                                         \
    unsigned long val = (key);                                                 \
    DEBUG_HASH("key:%x bits:%d val:%d\n", key, bits, (val >> (64 - (bits))));  \
    (val >> (64 - (bits)));                                                    \
    })

#define hash_add(ht, n, key)                                                   \
    hlist_add_head(n, &ht[hash_func(key, HASH_BITS(ht))])

#define hash_del(n)                                                            \
    hlist_del_init(n)

#define hash_for_each_safe(ht, i, n, pos, member)                              \
    for ((i) = 0, pos = NULL; pos == NULL && (i) < HASH_SIZE(ht); (i)++)       \
        hlist_for_each_entry_safe(pos, n, &(ht)[i], member)

#define hash_for_each_possible(ht, pos, member, key)                           \
    hlist_for_each_entry(pos, &(ht)[hash_func(key, HASH_BITS(ht))], member)

static inline int hash_empty
    (
    struct hlist_head *ht
    )
    {
    unsigned int i;

    for (i = 0; i < HASH_SIZE(ht); i++)
        if (!hlist_empty(&ht[i]))
            return 0;
    return 1;
    }

#endif /* _VXOAL_LIST_H */
