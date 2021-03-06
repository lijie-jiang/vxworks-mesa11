/*
 * Copyright © 2009,2012 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22dec14,yat  Port Mesa to VxWorks 7 (US24705)
*/

#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#include <stdlib.h>
#if defined(__vxworks)
#include <sys/types.h>
#else
#include <inttypes.h>
#endif /* __vxworks */
#include <stdbool.h>
#include "c99_compat.h"
#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hash_entry {
   uint32_t hash;
   const void *key;
   void *data;
};

struct hash_table {
   struct hash_entry *table;
   uint32_t (*key_hash_function)(const void *key);
   bool (*key_equals_function)(const void *a, const void *b);
   const void *deleted_key;
   uint32_t size;
   uint32_t rehash;
   uint32_t max_entries;
   uint32_t size_index;
   uint32_t entries;
   uint32_t deleted_entries;
};

struct hash_table *
_mesa_hash_table_create(void *mem_ctx,
                        uint32_t (*key_hash_function)(const void *key),
                        bool (*key_equals_function)(const void *a,
                                                    const void *b));
void _mesa_hash_table_destroy(struct hash_table *ht,
                              void (*delete_function)(struct hash_entry *entry));
void _mesa_hash_table_set_deleted_key(struct hash_table *ht,
                                      const void *deleted_key);

struct hash_entry *
_mesa_hash_table_insert(struct hash_table *ht, const void *key, void *data);
struct hash_entry *
_mesa_hash_table_insert_pre_hashed(struct hash_table *ht, uint32_t hash,
                                   const void *key, void *data);
struct hash_entry *
_mesa_hash_table_search(struct hash_table *ht, const void *key);
struct hash_entry *
_mesa_hash_table_search_pre_hashed(struct hash_table *ht, uint32_t hash,
                                  const void *key);
void _mesa_hash_table_remove(struct hash_table *ht,
                             struct hash_entry *entry);

struct hash_entry *_mesa_hash_table_next_entry(struct hash_table *ht,
                                               struct hash_entry *entry);
struct hash_entry *
_mesa_hash_table_random_entry(struct hash_table *ht,
                              bool (*predicate)(struct hash_entry *entry));

uint32_t _mesa_hash_data(const void *data, size_t size);
uint32_t _mesa_hash_string(const char *key);
bool _mesa_key_string_equal(const void *a, const void *b);
bool _mesa_key_pointer_equal(const void *a, const void *b);

static inline uint32_t _mesa_key_hash_string(const void *key)
{
   return _mesa_hash_string((const char *)key);
}

static inline uint32_t _mesa_hash_pointer(const void *pointer)
{
   return _mesa_hash_data(&pointer, sizeof(pointer));
}

static const uint32_t _mesa_fnv32_1a_offset_bias = 2166136261u;

static inline uint32_t
_mesa_fnv32_1a_accumulate_block(uint32_t hash, const void *data, size_t size)
{
   const uint8_t *bytes = (const uint8_t *)data;

   while (size-- != 0) {
      hash ^= *bytes;
      hash = hash * 0x01000193;
      bytes++;
   }

   return hash;
}

#define _mesa_fnv32_1a_accumulate(hash, expr) \
   _mesa_fnv32_1a_accumulate_block(hash, &(expr), sizeof(expr))

/**
 * This foreach function is safe against deletion (which just replaces
 * an entry's data with the deleted marker), but not against insertion
 * (which may rehash the table, making entry a dangling pointer).
 */
#define hash_table_foreach(ht, entry)                   \
   for (entry = _mesa_hash_table_next_entry(ht, NULL);  \
        entry != NULL;                                  \
        entry = _mesa_hash_table_next_entry(ht, entry))

#ifdef __cplusplus
} /* extern C */
#endif

#endif /* _HASH_TABLE_H */
