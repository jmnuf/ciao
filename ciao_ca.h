/**
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
 *
 * @file ciao_ca.h
 * @brief Generic dynamic arrays using macros for ease of use.
 *
 * This library provides a macro-based approach to creating and managing
 * dynamic arrays of any type. Define an array using DynamicArray(T) where
 * T is your element type, then use the provided macros for operations.
 *
 * @note Requires CIAO_CAH_REALLOC and CIAO_CAH_FREE macros to be defined before including
 *       the implementation section.
 *
 * @par Usage Example:
 * @code
 * #define CIAO_CAH_REALLOC realloc
 * #define CIAO_CAH_FREE free
 * #define CIAO_CAH_IMPLEMENTATION
 * #define CIAO_STRIP_PREFIX
 * #include "ciao_ca.h"
 *
 * DynamicArray(int) numbers = {0};
 * da_push(numbers, 42);
 * da_push(numbers, 100);
 * da_foreach(int, it, numbers) {
 *     printf("%d\n", *it);
 * }
 * da_free(numbers);
 * @endcode
 */

#ifndef __CIAO_CA_H
#define __CIAO_CA_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Default initial capacity for newly allocated arrays.
 *
 * Can be overridden before including the header.
 */
#ifndef CIAO_CAH_INIT_CAP
#    define CIAO_CAH_INIT_CAP (64)
#endif // CIAO_CAH_INIT_CAP

/**
 * Defines a dynamic array structure for a given type.
 *
 * @param T The element type for the array.
 *
 * Creates a struct containing:
 * - items: Pointer to the array data.
 * - len: Current number of elements.
 * - cap: Total capacity available.
 *
 * @par Example:
 * @code
 * DynamicArray(int) my_array;
 * my_array.items = NULL;
 * my_array.len = 0;
 * my_array.cap = 0;
 * @endcode
 */
#define CIAO_DynamicArray(T) struct { T *items; size_t len; size_t cap; }

/**
 * Returns the size of elements in a dynamic array.
 * @param da Pointer to the dynamic array.
 * @return Size in bytes of a single element.
 */
#define CIAO_DA_ITEM_SIZE(da) sizeof(*(da)->items)

/**
 * Returns a pointer to the element at the given index.
 * @param da Pointer to the dynamic array.
 * @param i Zero-based index of the element.
 * @return Pointer to the element at index i.
 */
#define ciao_da_at(da, i) (&(da)->items[(i)])

/**
 * Gets the element at the given index by value.
 * @param da Pointer to the dynamic array.
 * @param i Zero-based index of the element.
 * @return The element value at index i.
 */
#define ciao_da_get(da, i) ((da)->items[(i)])

/**
 * Returns a pointer to the last element, or NULL if empty.
 * @param da Pointer to the dynamic array.
 * @return Pointer to the last element, or NULL if len is 0.
 */
#define ciao_da_last(da) ((da)->len == 0 ? NULL : &(da)->items[(da)->len-1])
#define ciao_da_lasti(da) ((da)->len <= 1 ? 0 : (da)->len-1)

#define ciao_da_first(da) ((da)->len == 0 ? NULL : &(da)->items[0])

/**
 * Ensures the array has capacity for at least the specified number of items.
 *
 * Capacity grows exponentially (doubles) until it meets or exceeds the requested amount.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param item_size Size of each element in bytes.
 * @param amount Minimum capacity to ensure.
 * @return true if successful or current capacity is sufficient, false on allocation failure.
 */
bool ciao_da_void_reserve(void **items_ref, size_t *cap, size_t item_size, size_t amount);

/**
 * Frees the array memory and resets length and capacity.
 *
 * @param[in,out] items_ref Pointer to the items pointer (set to NULL on free).
 * @param[in,out] len Pointer to the length variable (set to 0).
 * @param[in,out] cap Pointer to the capacity variable (set to 0).
 */
void ciao_da_void_free(void **items_ref, size_t *len, size_t *cap);

/**
 * Reallocates to exactly fit the current length (no extra capacity).
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param len Exact size to allocate for.
 * @return true on success, false on allocation failure.
 */
bool ciao_da_void_reserve_exact(void **items_ref, size_t *cap, size_t len);

/**
 * Appends a single item to the end of the array.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] len Pointer to the length variable.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param item Pointer to the item to append.
 * @param item_size Size of each element in bytes.
 * @return true on success, false on allocation failure.
 */
bool ciao_da_void_push(void **items_ref, size_t *len, size_t *cap, const void *item, size_t item_size);

/**
 * Inserts a single item at the specified index, shifting elements right.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] len Pointer to the length variable.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param item_ref Pointer to the item to insert.
 * @param item_size Size of each element in bytes.
 * @param index Position to insert at (0 to len, inclusive).
 * @return true on success, false if index is out of range or allocation fails.
 */
bool ciao_da_void_insert(void **items_ref, size_t *len, size_t *cap, const void *item_ref, size_t item_size, size_t index);

/**
 * Appends multiple items from an array to the dynamic array.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] len Pointer to the length variable.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param items_arr Pointer to the source array of items.
 * @param item_size Size of each element in bytes.
 * @param items_count Number of items to append.
 * @return true on success, false if source overlaps with destination or allocation fails.
 */
bool ciao_da_void_push_many(void **items_ref, size_t *len, size_t *cap, const void *items_arr, size_t item_size, size_t items_count);

/**
 * Removes and optionally returns the last element.
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param[out] item_out_ref Pointer to store the popped item (can be NULL to discard).
 * @param item_size Size of each element in bytes.
 * @return true if an item was popped, false if array is empty.
 */
bool ciao_da_void_pop(void *items, size_t *len, void *item_out_ref, size_t item_size);

/**
 * Removes and optionally returns the element at index using swap-remove.
 *
 * Swaps the element with the last element, then removes it. O(1) but does not preserve order.
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param[out] item_out_ref Pointer to store the removed item (can be NULL).
 * @param item_size Size of each element in bytes.
 * @param index Position to remove at.
 * @return true if an item was removed, false if index is out of range.
 */
bool ciao_da_void_pop_at_unordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index);

/**
 * Removes and optionally returns the element at index, preserving order.
 *
 * Shifts all subsequent elements left. O(n).
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param[out] item_out_ref Pointer to store the removed item (can be NULL).
 * @param item_size Size of each element in bytes.
 * @param index Position to remove at.
 * @return true if an item was removed, false if index is out of range.
 */
bool ciao_da_void_pop_at_ordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index);

/**
 * Swaps two elements in the array by index.
 *
 * @param items Pointer to the items array.
 * @param len Current length of the array.
 * @param item_size Size of each element in bytes.
 * @param i First index.
 * @param j Second index.
 * @return true on success, false if indices are out of range.
 */
bool ciao_da_void_swap(void *items, size_t len, size_t item_size, size_t i, size_t j);

/**
 * Removes the element at index, preserving order by shifting elements left.
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param item_size Size of each element in bytes.
 * @param index Position to remove at.
 */
void ciao_da_void_remove_ordered(void *items, size_t *len, size_t item_size, size_t index);

/**
 * Removes the element at index using swap-remove (order not preserved).
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param item_size Size of each element in bytes.
 * @param index Position to remove at.
 */
void ciao_da_void_remove_unordered(void *items, size_t *len, size_t item_size, size_t index);

/**
 * Finds the first element matching a predicate.
 *
 * @param items Pointer to the items array.
 * @param len Number of elements to search.
 * @param item_size Size of each element in bytes.
 * @param predicate Function that returns true if item matches.
 * @param[out] index Pointer to store the found index (can be NULL).
 * @param user_context User data passed to predicate.
 * @return true if a match was found, false otherwise.
 */
bool ciao_da_void_find(const void *items, size_t len, size_t item_size, bool (*predicate)(const void *item, const void *user_context), size_t *index, const void *user_context);

/**
 * Finds the first element matching a predicate in a typed dynamic array.
 *
 * @param da Pointer to the dynamic array.
 * @param predicate Function that returns true if item matches.
 * @param[out] index Pointer to store the found index (can be NULL).
 * @param ctx User context passed to predicate.
 * @return true if a match was found, false otherwise.
 */
#define ciao_da_find(da, predicate, index, ctx) ciao_da_void_find((da)->items, (da)->len, CIAO_DA_ITEM_SIZE(da), (predicate), (index), (ctx))

/**
 * Ensures the array has capacity for at least the specified number of items.
 *
 * @param da Pointer to the dynamic array.
 * @param count Minimum total capacity to ensure.
 * @return true on success, false on allocation failure.
 */
#define ciao_da_reserve(da, count) ciao_da_void_reserve((void**)&(da)->items, &(da)->cap, CIAO_DA_ITEM_SIZE(da), (count))

/**
 * Reallocates to exactly fit the current length (no extra capacity).
 *
 * @param da Pointer to the dynamic array.
 * @param count Expected final length (typically (da)->len).
 * @return true on success, false on allocation failure.
 */
#define ciao_da_reserve_exact(da, count) ciao_da_void_reserve_exact((void**)&(da)->items, &(da)->cap, (count))

/**
 * Shrinks capacity to match current length, freeing excess memory.
 *
 * @param da Pointer to the dynamic array.
 * @return true on success, false on allocation failure.
 */
#define ciao_da_shrink(da) ciao_da_reserve_exact((da), ((da)->len))

/**
 * Ensures space for additional elements beyond current length.
 *
 * @param da Pointer to the dynamic array.
 * @param additional_count Number of extra elements to accommodate.
 * @return true on success, false on allocation failure.
 */
#define ciao_da_ensure(da, additional_count) ciao_da_reserve((da), ((da)->len + (additional_count)))

/**
 * Pushes a value of an explicitly specified type onto the array.
 *
 * @param T The element type.
 * @param da Pointer to the dynamic array.
 * @param value The value to append.
 * @return true on success, false on allocation failure.
 */
#define ciao_da_push_typed(T, da, value) ciao_da_void_push((void**)&(da)->items, &(da)->len, &(da)->cap, (T[]){(value)}, CIAO_DA_ITEM_SIZE(da))

/**
 * Inserts a value of an explicitly specified type at a given index.
 *
 * @param T The element type.
 * @param da Pointer to the dynamic array.
 * @param value The value to insert.
 * @param index Position to insert at.
 * @return true on success, false if index is out of range or allocation fails.
 */
#define ciao_da_insert_typed(T, da, value, index) ciao_da_void_insert((void**)&(da)->items, &(da)->len, &(da)->cap, (T[]){(value)}, CIAO_DA_ITEM_SIZE(da), (index))

/**
 * Swaps two elements in the array by index.
 *
 * @param da Pointer to the dynamic array.
 * @param i First index.
 * @param j Second index.
 * @return true on success, false if indices are out of range.
 */
#define ciao_da_swap(da, i, j) ciao_da_void_swap((da)->items, (da)->len, CIAO_DA_ITEM_SIZE(da), (i), (j))

#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1930)
   // GCC, Clang, and MSVC 2022+ support __typeof__
#  define CIAO_DA_TYPEOF(x) __typeof__(x)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
   // C23 Standard support
#  define CIAO_DA_TYPEOF(x) typeof(x)
#endif

#ifdef CIAO_DA_TYPEOF
/**
 * Pushes a value onto the array, inferring type from the array.
 *
 * Requires compiler support for typeof (GCC, Clang, or MSVC 2022+).
 *
 * @param da Pointer to the dynamic array.
 * @param value The value to append (type inferred from array element type).
 * @return true on success, false on allocation failure.
 */
#  define ciao_da_push(da, value) ciao_da_push_typed(CIAO_DA_TYPEOF(*(da)->items), (da), (value))
/**
 * Inserts a value at a given index, inferring type from the array.
 *
 * Requires compiler support for typeof (GCC, Clang, or MSVC 2022+).
 *
 * @param da Pointer to the dynamic array.
 * @param value The value to insert.
 * @param index Position to insert at.
 * @return true on success, false if index is out of range or allocation fails.
 */
#  define ciao_da_insert(da, value, index) ciao_da_insert_typed(CIAO_DA_TYPEOF(*(da)->items), (da), (value), (index))
#elif !defined(CIAO_CAH_IGNORE_MISSING_TYPEOF)
#  warning "Compiler does not support known typeof extraction unable to create ciao_da_push(da, value) and ciao_da_insert(da, value, index) macros"
#endif // CIAO_DA_TYPEOF

/**
 * Pushes elements from a C array onto the dynamic array.
 *
 * @param da Pointer to the dynamic array.
 * @param array Source C array.
 * @param arr_len Number of elements in the source array.
 * @return true on success, false if allocation fails.
 */
#define ciao_da_push_arr(da, array, arr_len) ciao_da_void_push_many((void**)&(da)->items, &(da)->len, &(da)->cap, (array), CIAO_DA_ITEM_SIZE(da), arr_len)

/**
 * Pushes all elements from one dynamic array onto another.
 *
 * @param ciao_da_a Destination dynamic array.
 * @param ciao_da_b Source dynamic array.
 * @return true on success, false if allocation fails.
 */
#define ciao_da_push_da(da_a, da_b) ciao_da_push_arr((da_a), ((da_b)->items), ((da_b)->len))

/**
 * Pops the last element from the array.
 *
 * @param da Pointer to the dynamic array.
 * @param item_out Variable to store the popped element.
 * @return true if an element was popped, false if array is empty.
 */
#define ciao_da_pop(da, item_out) ciao_da_void_pop((da)->items, &(da)->len, (item_out), CIAO_DA_ITEM_SIZE(da))

/**
 * Pops the element at a specific index, preserving order.
 *
 * @param da Pointer to the dynamic array.
 * @param index Position to pop at.
 * @param item_out Variable to store the popped element.
 * @return true if an element was popped, false if index is out of range.
 */
#define ciao_da_pop_at_ordered(da, index, item_out) ciao_da_void_pop_at_ordered((da)->items, &(da)->len, (item_out), CIAO_DA_ITEM_SIZE(da), (index))

/**
 * Pops the element at a specific index using swap-remove.
 *
 * @param da Pointer to the dynamic array.
 * @param index Position to pop at.
 * @param item_out Variable to store the popped element.
 * @return true if an element was popped, false if index is out of range.
 */
#define ciao_da_pop_at_unordered(da, index, item_out) ciao_da_void_pop_at_unordered((da)->items, &(da)->len, (item_out), CIAO_DA_ITEM_SIZE(da), (index))

/**
 * Frees all memory associated with the array.
 *
 * @param da Pointer to the dynamic array.
 */
#define ciao_da_free(da) ciao_da_void_free((void**)&(da)->items, &(da)->len, &(da)->cap)

/**
 * Sets the length to zero, effectively clearing all elements without freeing memory.
 *
 * @param da Pointer to the dynamic array.
 */
#define ciao_da_clear(da) do { (da)->len = 0; } while (0)

/**
 * Clears the destination array and copies all elements from the source.
 *
 * @param dest_da Destination dynamic array (cleared first).
 * @param src_da Source dynamic array.
 */
#define ciao_da_copy(dest_da, src_da) ((dest_da)->len = 0, ciao_da_push_da((dest_da), (src_da)))

/**
 * Iterates over all elements in the array with a typed pointer.
 *
 * @param T Element type.
 * @param it Iterator variable name.
 * @param da Pointer to the dynamic array.
 *
 * @par Example:
 * @code
 * ciao_da_foreach(int, it, my_array) {
 *     printf("%d\n", *it);
 * }
 * @endcode
 */
#define ciao_da_foreach(T, it, da) for (T *it = (da)->items; it < (da)->items + (da)->len; ++it)

/**
 * Iterates over all elements in the array with a const typed pointer.
 *
 * @param T Element type (const).
 * @param it Iterator variable name.
 * @param da Pointer to the dynamic array.
 */
#define ciao_da_foreach_const(T, it, da) for (const T *it = (da)->items; it < (da)->items + (da)->len; ++it)

/**
 * Iterates over array indices.
 *
 * @param it Index variable name (size_t).
 * @param da Pointer to the dynamic array.
 *
 * @par Example:
 * @code
 * ciao_da_fori(i, my_array) {
 *     printf("%zu: %d\n", i, ciao_da_get(my_array, i));
 * }
 * @endcode
 */
#define ciao_da_fori(it, da) for (size_t it = 0; it < (da)->len; ++it)

#endif // __CIAO_CA_H

#if (defined(CIAO_CAH_STRIP_PREFIX) || defined(CIAO_STRIP_PREFIX)) && !defined(__CIAO_CAH_STRIP_PREFIX)
#define __CIAO_CAH_STRIP_PREFIX

#define CAH_INIT_CAP CIAO_CAH_INIT_CAP
#define DynamicArray CIAO_DynamicArray
#define DA_ITEM_SIZE CIAO_DA_ITEM_SIZE
#define da_at ciao_da_at
#define da_get ciao_da_get
#define da_last ciao_da_last
#define da_lasti ciao_da_lasti
#define da_first ciao_da_first
#define da_void_reserve ciao_da_void_reserve
#define da_void_free ciao_da_void_free
#define da_void_reserve_exact ciao_da_void_reserve_exact
#define da_void_push ciao_da_void_push
#define da_void_insert ciao_da_void_insert
#define da_void_push_many ciao_da_void_push_many
#define da_void_pop ciao_da_void_pop
#define da_void_pop_at_unordered ciao_da_void_pop_at_unordered
#define da_void_pop_at_ordered ciao_da_void_pop_at_ordered
#define da_void_swap ciao_da_void_swap
#define da_void_remove_ordered ciao_da_void_remove_ordered
#define da_void_remove_unordered ciao_da_void_remove_unordered
#define da_void_find ciao_da_void_find
#define da_find ciao_da_find
#define da_reserve ciao_da_reserve
#define da_reserve_exact ciao_da_reserve_exact
#define da_shrink ciao_da_shrink
#define da_ensure ciao_da_ensure
#define da_push_typed ciao_da_push_typed
#define da_insert_typed ciao_da_insert_typed
#define da_swap ciao_da_swap
#ifdef CIAO_DA_TYPEOF
#    define da_push ciao_da_push
#    define da_insert ciao_da_insert
#endif // CIAO_DA_TYPEOF
#define da_push_arr ciao_da_push_arr
#define da_push_da ciao_da_push_da
#define da_pop ciao_da_pop
#define da_pop_at_ordered ciao_da_pop_at_ordered
#define da_pop_at_unordered ciao_da_pop_at_unordered
#define da_free ciao_da_free
#define da_clear ciao_da_clear
#define da_copy ciao_da_copy
#define da_foreach ciao_da_foreach
#define da_foreach_const ciao_da_foreach_const
#define da_fori ciao_da_fori

#endif // CIAO_CAH_STRIP_PREFIX

#ifdef CIAO_CAH_IMPLEMENTATION

#ifndef CIAO_CAH_MEM_REALLOC
static_assert(false, "CIAO_CAH_MEM_REALLOC macro requires to be specified");
#endif //  CIAO_CAH_MEM_REALLOC

#ifndef CIAO_CAH_MEM_FREE
static_assert(false, "CIAO_CAH_MEM_FREE macro requires to be specified");
#endif //  CIAO_CAH_MEM_FREE

static inline void *ciao__cah_mem_copy(void *restrict dest, void *restrict src, size_t n) {
  char *dest_buf = (char*)dest;
  char *src_buf  = (char*)src;
  for (size_t i = 0; i < n; ++i) {
    dest_buf[i] = src_buf[i];
  }
  return dest_buf;
}

static inline void *ciao__cah_mem_move(void *dest, const void *src, size_t n) {
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

bool ciao_da_void_reserve(void **items_ref, size_t *cap, size_t item_size, size_t amount) {
  if (*cap >= amount) return true;
  size_t ncap = *cap == 0 ? CIAO_CAH_INIT_CAP : (*cap);
  while (ncap < amount) {
    if (ncap > SIZE_MAX / 2) {
      ncap = amount;
      break;
    }
    ncap *= 2;
  }
  void *nptr = CIAO_CAH_MEM_REALLOC(*items_ref, ncap * item_size);
  // Last ditch effort
  if (nptr == NULL && ncap > amount) {
    ncap = amount;
    nptr = (char*)CIAO_CAH_MEM_REALLOC(*items_ref, ncap);
  }
  if (nptr == NULL) return false;
  *items_ref = nptr;
  *cap = ncap;
  return true;
}

void ciao_da_void_free(void **items_ref, size_t *len, size_t *cap) {
  if (*items_ref) {
    CIAO_CAH_MEM_FREE(*items_ref);
    *items_ref = NULL;
  }
  *len = 0;
  *cap = 0;
}

bool ciao_da_void_reserve_exact(void **items_ref, size_t *cap, size_t len) {
  void *new_ptr = CIAO_CAH_MEM_REALLOC(*items_ref, len);
  if (!new_ptr) return false;
  *items_ref = new_ptr;
  *cap = len;
  return true;
}

bool ciao_da_void_push(void **items_ref, size_t *len, size_t *cap, const void *item, size_t item_size) {
  size_t new_len = (*len) + 1;
  if (!ciao_da_void_reserve(items_ref, cap, item_size, new_len)) return false;
  char *items = (char*)*items_ref;
  ciao__cah_mem_copy(items + item_size * (*len), (void*)item, item_size);
  *len = new_len;
  return true;
}

bool ciao_da_void_insert(void **items_ref, size_t *len, size_t *cap, const void *item_ref, size_t item_size, size_t index) {
  if (index > *len) return false;
  size_t new_len = (*len) + 1;
  if (!ciao_da_void_reserve(items_ref, cap, item_size, new_len)) return false;
  char *items = (char*)*items_ref;
  if (index < *len) {
    size_t move_count = (*len - index) * item_size;
    ciao__cah_mem_move(items + (index + 1) * item_size, items + index * item_size, move_count);
  }
  ciao__cah_mem_copy(items + index * item_size, (void*)item_ref, item_size);
  *len = new_len;
  return true;
}

bool ciao_da_void_push_many(void **items_ref, size_t *len, size_t *cap, const void *items_arr, size_t item_size, size_t items_count) {
  size_t new_len = (*len) + items_count;
  if (*items_ref == items_arr) return false;
  if (!ciao_da_void_reserve(items_ref, cap, item_size, new_len)) return false;
  char *da_items = (char*)*items_ref;
  da_items += (*len) * item_size;
  ciao__cah_mem_copy(da_items, (void*)items_arr, items_count * item_size);
  *len = new_len;
  return true;
}

bool ciao_da_void_pop(void *items, size_t *len, void *item_out_ref, size_t item_size) {
  if (*len == 0) return false;
  size_t new_len = (*len) - 1;
  char *item_in = ((char*)items) + (new_len * item_size);
  if (item_out_ref != NULL) ciao__cah_mem_copy(item_out_ref, item_in, item_size);
  *len = new_len;
  return true;
}

bool ciao_da_void_pop_at_unordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index) {
  if (index >= *len) return false;
  if (index < (*len - 1)) {
    if (!ciao_da_void_swap(items, *len, item_size, index, *len - 1)) return false;
  }
  return ciao_da_void_pop(items, len, item_out_ref, item_size);
}

bool ciao_da_void_pop_at_ordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index) {
  if (index >= *len) return false;
  if (item_out_ref) ciao__cah_mem_copy(item_out_ref, (char*)items + (index * item_size), item_size);
  ciao_da_void_remove_ordered(items, len, item_size, index);
  return true;
}

bool ciao_da_void_swap(void *items, size_t len, size_t item_size, size_t i, size_t j) {
  if (i > len || j > len) return false;
  char temp[item_size];
  char *buf = (char*)items;
  char *item_a = buf + (i * item_size);
  char *item_b = buf + (j * item_size);
  ciao__cah_mem_copy(temp, item_a, item_size);
  ciao__cah_mem_copy(item_a, item_b, item_size);
  ciao__cah_mem_copy(item_b, temp, item_size);
  return true;
}

void ciao_da_void_remove_ordered(void *items, size_t *len, size_t item_size, size_t index) {
  if (*len == 0 || index >= *len) return;
  if (index < (*len - 1)) {
    size_t move_count = (*len - index - 1) * item_size;
    ciao__cah_mem_move(
      (char*)items + index * item_size,
      (char*)items + (index+1) * item_size,
      move_count
    );
  }
  *len -= 1;
}

void ciao_da_void_remove_unordered(void *items, size_t *len, size_t item_size, size_t index) {
  if (*len == 0 || index >= *len) return;
  if (index < (*len - 1)) ciao_da_void_swap(items, *len, item_size, index, *len - 1);
  *len -= 1;
}

bool ciao_da_void_find(const void *items, size_t len, size_t item_size, bool (*predicate)(const void *item, const void *user_context), size_t *index, const void *user_context) {
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


#endif // CIAO_CAH_IMPLEMENTATION
