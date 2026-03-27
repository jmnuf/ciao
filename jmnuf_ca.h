#ifndef __DYNAMIC_ARRAY_CA_H
#define __DYNAMIC_ARRAY_CA_H
/*
 * MIT License
 * 
 * Copyright (c) 2026 Jose Marco Nufio
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stddef.h>

#ifndef DA_INIT_CAP
#define DA_INIT_CAP (64)
#endif // DA_INIT_CAP

#define DynamicArray(T) struct { T *items; size_t len; size_t cap; }
#define DA_ITEM_SIZE(da) sizeof(*(da)->items)
#define da_at(da, i) (&(da)->items[(i)])
#define da_get(da, i) ((da)->items[(i)])
#define da_last(da) ((da)->len == 0 ? NULL : &(da)->items[(da)->len-1])

bool da_void_reserve(void **items_ref, size_t *cap, size_t item_size, size_t amount);
void da_void_free(void **items_ref, size_t *len, size_t *cap);
bool da_void_reserve_exact(void **items_ref, size_t *cap, size_t len);

bool da_void_push(void **items_ref, size_t *len, size_t *cap, const void *item, size_t item_size);
bool da_void_insert(void **items_ref, size_t *len, size_t *cap, const void *item_ref, size_t item_size, size_t index);
bool da_void_push_many(void **items_ref, size_t *len, size_t *cap, const void *items_arr, size_t item_size, size_t items_count);

bool da_void_pop(void *items, size_t *len, void *item_out_ref, size_t item_size);
bool da_void_pop_at_unordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index);
bool da_void_pop_at_ordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index);

bool da_void_swap(void *items, size_t len, size_t item_size, size_t i, size_t j);

void da_void_remove_ordered(void *items, size_t *len, size_t item_size, size_t index);
void da_void_remove_unordered(void *items, size_t *len, size_t item_size, size_t index);

bool da_void_find(const void *items, size_t len, size_t item_size, bool (*predicate)(const void *item, const void *user_context), size_t *index, const void *user_context);

#define da_find(da, predicate, ctx) da_void_find((da)->items, (da)->len, DA_ITEM_SIZE(da), (predicate), (ctx))

#define da_reserve(da, count) da_void_reserve((void**)&(da)->items, &(da)->cap, DA_ITEM_SIZE(da), (count))
#define da_reserve_exact(da, count) da_void_reserve_exact((void**)&(da)->items, &(da)->cap, (count))
#define da_shrink(da) da_reserve_exact((da), ((da)->len))
#define da_ensure(da, additional_count) da_reserve((da), ((da)->len + (additional_count)))

#define da_push_typed(T, da, value) da_void_push((void**)&(da)->items, &(da)->len, &(da)->cap, &(T){(value)}, DA_ITEM_SIZE(da))
#define da_insert_typed(T, da, value, index) da_void_insert((void**)&(da)->items, &(da)->len, &(da)->cap, &(T){(value)}, DA_ITEM_SIZE(da), (index))

#define da_swap(da, i, j) da_void_swap((da)->items, (da)->len, DA_ITEM_SIZE(da), (i), (j))

#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1930)
   // GCC, Clang, and MSVC 2022+ support __typeof__
#  define DA_TYPEOF(x) __typeof__(x)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
   // C23 Standard support
#  define DA_TYPEOF(x) typeof(x)
#endif

#ifdef DA_TYPEOF
#  define da_push(da, value) da_push_typed(DA_TYPEOF(*(da)->items), (da), (value))
#  define da_insert(da, value, index) da_insert_typed(DA_TYPEOF(*(da)->items), (da), (value), (index))
#elif !defined(DA_IGNORE_MISSING_TYPEOF)
#  warning "Compiler does not support known typeof extraction unable to create da_push(da, value) and da_insert(da, value, index) macros"
#endif // DA_TYPEOF

#define da_push_arr(da, array, arr_len) da_void_push_many((void**)&(da)->items, &(da)->len, &(da)->cap, (array), DA_ITEM_SIZE(da), arr_len)
#define da_push_da(da_a, da_b) da_push_arr((da_a), ((da_b)->items), ((da_b)->len))
#define da_pop(da, item_out) da_void_pop((da)->items, &(da)->len, (item_out), DA_ITEM_SIZE(da))
#define da_pop_at_ordered(da, index, item_out) da_void_pop_at_ordered((da)->items, &(da)->len, (item_out), DA_ITEM_SIZE(da), (index))
#define da_pop_at_unordered(da, index, item_out) da_void_pop_at_unordered((da)->items, &(da)->len, (item_out), DA_ITEM_SIZE(da), (index))

#define da_free(da) da_void_free(&(da)->items, &(da)->len, &(da)->cap)

#define da_clear(da) do { (da)->len = 0; } while (0)

#define da_copy(dest_da, src_da) (da_clear(dest_da), da_push_da((dest_da), (src_da)))

#define da_foreach(T, it, da) for (T *it = (da)->items; it < (da)->items + (da)->len; ++it)
#define da_foreach_const(T, it, da) for (const T *it = (da)->items; it < (da)->items + (da)->len; ++it)
#define da_fori(it, da) for (size_t it = 0; it < (da)->len; ++it)

#endif // __DYNAMIC_ARRAY_CA_H

#ifdef JMNUF_CAH_IMPLEMENTATION

#ifndef DA_REALLOC
static_assert(false, "DA_REALLOC macro requires to be specified");
#endif //  DA_REALLOC

#ifndef DA_FREE
static_assert(false, "DA_FREE macro requires to be specified");
#endif //  DA_FREE

static inline void *da__memcpy(void restrict *dest, void restrict *src, size_t n) {
  char *dest_buf = (char*)dest;
  char *src_buf  = (char*)src;
  for (size_t i = 0; i < n; ++i) {
    dest_buf[i] = src_buf[i];
  }
  return dest_buf;
}

static inline void *da__memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else if (d > s) {
        d += n; s += n;
        while (n--) *--d = *--s;
    }
    return dest;
}

bool da_void_reserve(void **items_ref, size_t *cap, size_t item_size, size_t amount) {
  if (*cap >= amount) return true;
  size_t new_cap = *cap;
  size_t new_cap = new_cap == 0 ? DA_INIT_CAP : (new_cap * 2);
  while (new_cap < amount) new_cap *= 2;
  void *new_items = DA_REALLOC(*items_ref, new_cap * item_size);
  if (new_items == NULL) return false;
  *items_ref = new_items;
  *cap = new_cap;
  return true;
}

void da_void_free(void **items_ref, size_t *len, size_t *cap) {
  if (*items_ref) {
    DA_FREE(*items_ref);
    *items_ref = NULL;
  }
  *len = 0;
  *cap = 0;
}

bool da_void_reserve_exact(void **items_ref, size_t *cap, size_t len) {
  void *new_ptr = DA_REALLOC(*items_ref, len);
  if (!new_ptr) return false;
  *items_ref = new_ptr;
  *cap = len;
  return true;
}

bool da_void_push(void **items_ref, size_t *len, size_t *cap, const void *item, size_t item_size) {
  size_t new_len = (*len) + 1;
  if (!da_void_reserve(items_ref, cap, item_size, new_len)) return false;
  da__memcpy((char*)items + item_size * (*len), item, item_size);
  *len = new_len;
  return true;
}

bool da_void_insert(void **items_ref, size_t *len, size_t *cap, const void *item_ref, size_t item_size, size_t index) {
  if (index > *len) return false;
  size_t new_len = (*len) + 1;
  if (!da_void_reserve(items_ref, cap, item_size, new_len)) return false;
  char *items = (char*)*items_ref;
  if (index < *len) {
    size_t move_count = (*len - index) * item_size;
    da__memmove(items + (index + 1) * item_size, items + index * item_size, move_count);
  }
  da__memcpy(items + index * item_size, item_ref, item_size);
  *len = new_len;
  return true;
}

bool da_void_push_many(void **items_ref, size_t *len, size_t *cap, const void *items_arr, size_t item_size, size_t items_count) {
  size_t new_len = (*len) + items_count;
  if (*items_ref == items_arr) return false;
  if (!da_void_reserve(items_ref, cap, item_size, new_len)) return false;
  char *da_items = (char*)*items_ref;
  da_items += (*len) * item_size;
  da__memcpy(da_items, items_arr, items_count * item_size);
  *len = new_len;
  return true;
}

bool da_void_pop(void *items, size_t *len, void *item_out_ref, size_t item_size) {
  if (*len == 0) return false;
  size_t new_len = (*len) - 1;
  char *item_in = ((char*)items) + (new_len * item_size);
  if (item_out_ref != NULL) da__memcpy(item_out_ref, item_in, item_size);
  *len = new_len;
  return true;
}

bool da_void_pop_at_unordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index) {
  if (index >= *len) return false;
  if (index < (*len - 1)) {
    if (!da_void_swap(items, *len, item_size, index, *len - 1)) return false;
  }
  return da_void_pop(items, len, item_out_ref, item_size);
}

bool da_void_pop_at_ordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index) {
  if (index >= *len) return false;
  if (item_out_ref) da__memcpy(item_out_ref, (char*)items + (index * item_size), item_size);
  da_void_remove_ordered(items, len, item_size, index);
  return true;
}

bool da_void_swap(void *items, size_t len, size_t item_size, size_t i, size_t j) {
  if (i > len || j > len) return false;
  char temp[item_size];
  char *buf = (char*)items;
  char *item_a = buf + (i * item_size);
  char *item_b = buf + (j * item_size);
  da__memcpy(temp, item_a, item_size);
  da__memcpy(item_a, item_b, item_size);
  da__memcpy(item_b, temp, item_size);
  return true;
}

void da_void_remove_ordered(void *items, size_t *len, size_t item_size, size_t index) {
  if (*len == 0 || index >= *len) return;
  if (index < (*len - 1)) {
    size_t move_count = (*len - index - 1) * item_size;
    da__memmove(
      (char*)items + index * item_size,
      (char*)items + (index+1) * item_size,
      move_count
    );
  }
  *len -= 1;
}

void da_void_remove_unordered(void *items, size_t *len, size_t item_size, size_t index) {
  if (*len == 0 || index >= *len) return;
  if (index < (*len - 1)) da_void_swap(items, *len, item_size, index, *len - 1);
  *len -= 1;
}

bool da_void_find(const void *items, size_t len, size_t item_size, bool (*predicate)(const void *item, const void *user_context), size_t *index, const void *user_context) {
  if (items == NULL || predicate == NULL) return false;
  const unsigned char *start = (const unsigned char*)items;
  for (size_t i = 0; i < len; ++i) {
    if (predicate(start + i * item_size, user_context)) {
      if (index) *index = i;
      return true;
    }
  }
  return false;
}


#endif // JMNUF_CAH_IMPLEMENTATION
