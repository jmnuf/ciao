/**
 * @file jmnuf_ca.h
 * @brief Type-safe generic dynamic array library for C.
 *
 * This library provides a macro-based approach to creating and managing
 * dynamic arrays of any type. Define an array using DynamicArray(T) where
 * T is your element type, then use the provided macros for type-safe operations.
 *
 * @note Requires DA_REALLOC and DA_FREE macros to be defined before including
 *       the implementation section.
 *
 * @par Usage Example:
 * @code
 * #define DA_REALLOC realloc
 * #define DA_FREE free
 * #define JMNUF_CAH_IMPLEMENTATION
 * #include "jmnuf_ca.h"
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
#include <stdbool.h>

/**
 * @brief Default initial capacity for newly allocated arrays.
 *
 * Can be overridden before including the header.
 */
#ifndef DA_INIT_CAP
#define DA_INIT_CAP (64)
#endif // DA_INIT_CAP

/**
 * @brief Defines a dynamic array structure for a given type.
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
#define DynamicArray(T) struct { T *items; size_t len; size_t cap; }

/**
 * @brief Returns the size of elements in a dynamic array.
 * @param da Pointer to the dynamic array.
 * @return Size in bytes of a single element.
 */
#define DA_ITEM_SIZE(da) sizeof(*(da)->items)

/**
 * @brief Returns a pointer to the element at the given index.
 * @param da Pointer to the dynamic array.
 * @param i Zero-based index of the element.
 * @return Pointer to the element at index i.
 */
#define da_at(da, i) (&(da)->items[(i)])

/**
 * @brief Gets the element at the given index by value.
 * @param da Pointer to the dynamic array.
 * @param i Zero-based index of the element.
 * @return The element value at index i.
 */
#define da_get(da, i) ((da)->items[(i)])

/**
 * @brief Returns a pointer to the last element, or NULL if empty.
 * @param da Pointer to the dynamic array.
 * @return Pointer to the last element, or NULL if len is 0.
 */
#define da_last(da) ((da)->len == 0 ? NULL : &(da)->items[(da)->len-1])

/**
 * @brief Ensures the array has capacity for at least the specified number of items.
 *
 * Capacity grows exponentially (doubles) until it meets or exceeds the requested amount.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param item_size Size of each element in bytes.
 * @param amount Minimum capacity to ensure.
 * @return true if successful or current capacity is sufficient, false on allocation failure.
 */
bool da_void_reserve(void **items_ref, size_t *cap, size_t item_size, size_t amount);

/**
 * @brief Frees the array memory and resets length and capacity.
 *
 * @param[in,out] items_ref Pointer to the items pointer (set to NULL on free).
 * @param[in,out] len Pointer to the length variable (set to 0).
 * @param[in,out] cap Pointer to the capacity variable (set to 0).
 */
void da_void_free(void **items_ref, size_t *len, size_t *cap);

/**
 * @brief Reallocates to exactly fit the current length (no extra capacity).
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param len Exact size to allocate for.
 * @return true on success, false on allocation failure.
 */
bool da_void_reserve_exact(void **items_ref, size_t *cap, size_t len);

/**
 * @brief Appends a single item to the end of the array.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] len Pointer to the length variable.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param item Pointer to the item to append.
 * @param item_size Size of each element in bytes.
 * @return true on success, false on allocation failure.
 */
bool da_void_push(void **items_ref, size_t *len, size_t *cap, const void *item, size_t item_size);

/**
 * @brief Inserts a single item at the specified index, shifting elements right.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] len Pointer to the length variable.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param item_ref Pointer to the item to insert.
 * @param item_size Size of each element in bytes.
 * @param index Position to insert at (0 to len, inclusive).
 * @return true on success, false if index is out of range or allocation fails.
 */
bool da_void_insert(void **items_ref, size_t *len, size_t *cap, const void *item_ref, size_t item_size, size_t index);

/**
 * @brief Appends multiple items from an array to the dynamic array.
 *
 * @param[in,out] items_ref Pointer to the items pointer.
 * @param[in,out] len Pointer to the length variable.
 * @param[in,out] cap Pointer to the capacity variable.
 * @param items_arr Pointer to the source array of items.
 * @param item_size Size of each element in bytes.
 * @param items_count Number of items to append.
 * @return true on success, false if source overlaps with destination or allocation fails.
 */
bool da_void_push_many(void **items_ref, size_t *len, size_t *cap, const void *items_arr, size_t item_size, size_t items_count);

/**
 * @brief Removes and optionally returns the last element.
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param[out] item_out_ref Pointer to store the popped item (can be NULL to discard).
 * @param item_size Size of each element in bytes.
 * @return true if an item was popped, false if array is empty.
 */
bool da_void_pop(void *items, size_t *len, void *item_out_ref, size_t item_size);

/**
 * @brief Removes and optionally returns the element at index using swap-remove.
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
bool da_void_pop_at_unordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index);

/**
 * @brief Removes and optionally returns the element at index, preserving order.
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
bool da_void_pop_at_ordered(void *items, size_t *len, void *item_out_ref, size_t item_size, size_t index);

/**
 * @brief Swaps two elements in the array by index.
 *
 * @param items Pointer to the items array.
 * @param len Current length of the array.
 * @param item_size Size of each element in bytes.
 * @param i First index.
 * @param j Second index.
 * @return true on success, false if indices are out of range.
 */
bool da_void_swap(void *items, size_t len, size_t item_size, size_t i, size_t j);

/**
 * @brief Removes the element at index, preserving order by shifting elements left.
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param item_size Size of each element in bytes.
 * @param index Position to remove at.
 */
void da_void_remove_ordered(void *items, size_t *len, size_t item_size, size_t index);

/**
 * @brief Removes the element at index using swap-remove (order not preserved).
 *
 * @param items Pointer to the items array.
 * @param[in,out] len Pointer to the length variable.
 * @param item_size Size of each element in bytes.
 * @param index Position to remove at.
 */
void da_void_remove_unordered(void *items, size_t *len, size_t item_size, size_t index);

/**
 * @brief Finds the first element matching a predicate.
 *
 * @param items Pointer to the items array.
 * @param len Number of elements to search.
 * @param item_size Size of each element in bytes.
 * @param predicate Function that returns true if item matches.
 * @param[out] index Pointer to store the found index (can be NULL).
 * @param user_context User data passed to predicate.
 * @return true if a match was found, false otherwise.
 */
bool da_void_find(const void *items, size_t len, size_t item_size, bool (*predicate)(const void *item, const void *user_context), size_t *index, const void *user_context);

/**
 * @brief Finds the first element matching a predicate in a typed dynamic array.
 *
 * @param da Pointer to the dynamic array.
 * @param predicate Function that returns true if item matches.
 * @param ctx User context passed to predicate.
 * @return true if a match was found, false otherwise.
 */
#define da_find(da, predicate, ctx) da_void_find((da)->items, (da)->len, DA_ITEM_SIZE(da), (predicate), (ctx))

/**
 * @brief Ensures the array has capacity for at least the specified number of items.
 *
 * @param da Pointer to the dynamic array.
 * @param count Minimum total capacity to ensure.
 * @return true on success, false on allocation failure.
 */
#define da_reserve(da, count) da_void_reserve((void**)&(da)->items, &(da)->cap, DA_ITEM_SIZE(da), (count))

/**
 * @brief Reallocates to exactly fit the current length (no extra capacity).
 *
 * @param da Pointer to the dynamic array.
 * @param count Expected final length (typically (da)->len).
 * @return true on success, false on allocation failure.
 */
#define da_reserve_exact(da, count) da_void_reserve_exact((void**)&(da)->items, &(da)->cap, (count))

/**
 * @brief Shrinks capacity to match current length, freeing excess memory.
 *
 * @param da Pointer to the dynamic array.
 * @return true on success, false on allocation failure.
 */
#define da_shrink(da) da_reserve_exact((da), ((da)->len))

/**
 * @brief Ensures space for additional elements beyond current length.
 *
 * @param da Pointer to the dynamic array.
 * @param additional_count Number of extra elements to accommodate.
 * @return true on success, false on allocation failure.
 */
#define da_ensure(da, additional_count) da_reserve((da), ((da)->len + (additional_count)))

/**
 * @brief Pushes a value of an explicitly specified type onto the array.
 *
 * @param T The element type.
 * @param da Pointer to the dynamic array.
 * @param value The value to append.
 * @return true on success, false on allocation failure.
 */
#define da_push_typed(T, da, value) da_void_push((void**)&(da)->items, &(da)->len, &(da)->cap, &(T){(value)}, DA_ITEM_SIZE(da))

/**
 * @brief Inserts a value of an explicitly specified type at a given index.
 *
 * @param T The element type.
 * @param da Pointer to the dynamic array.
 * @param value The value to insert.
 * @param index Position to insert at.
 * @return true on success, false if index is out of range or allocation fails.
 */
#define da_insert_typed(T, da, value, index) da_void_insert((void**)&(da)->items, &(da)->len, &(da)->cap, &(T){(value)}, DA_ITEM_SIZE(da), (index))

/**
 * @brief Swaps two elements in the array by index.
 *
 * @param da Pointer to the dynamic array.
 * @param i First index.
 * @param j Second index.
 * @return true on success, false if indices are out of range.
 */
#define da_swap(da, i, j) da_void_swap((da)->items, (da)->len, DA_ITEM_SIZE(da), (i), (j))

#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1930)
   // GCC, Clang, and MSVC 2022+ support __typeof__
#  define DA_TYPEOF(x) __typeof__(x)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
   // C23 Standard support
#  define DA_TYPEOF(x) typeof(x)
#endif

#ifdef DA_TYPEOF
/**
 * @brief Pushes a value onto the array, inferring type from the array.
 *
 * Requires compiler support for typeof (GCC, Clang, or MSVC 2022+).
 *
 * @param da Pointer to the dynamic array.
 * @param value The value to append (type inferred from array element type).
 * @return true on success, false on allocation failure.
 */
#  define da_push(da, value) da_push_typed(DA_TYPEOF(*(da)->items), (da), (value))
/**
 * @brief Inserts a value at a given index, inferring type from the array.
 *
 * Requires compiler support for typeof (GCC, Clang, or MSVC 2022+).
 *
 * @param da Pointer to the dynamic array.
 * @param value The value to insert.
 * @param index Position to insert at.
 * @return true on success, false if index is out of range or allocation fails.
 */
#  define da_insert(da, value, index) da_insert_typed(DA_TYPEOF(*(da)->items), (da), (value), (index))
#elif !defined(JMNUF_CAH_IGNORE_MISSING_TYPEOF)
#  warning "Compiler does not support known typeof extraction unable to create da_push(da, value) and da_insert(da, value, index) macros"
#endif // DA_TYPEOF

/**
 * @brief Pushes elements from a C array onto the dynamic array.
 *
 * @param da Pointer to the dynamic array.
 * @param array Source C array.
 * @param arr_len Number of elements in the source array.
 * @return true on success, false if allocation fails.
 */
#define da_push_arr(da, array, arr_len) da_void_push_many((void**)&(da)->items, &(da)->len, &(da)->cap, (array), DA_ITEM_SIZE(da), arr_len)

/**
 * @brief Pushes all elements from one dynamic array onto another.
 *
 * @param da_a Destination dynamic array.
 * @param da_b Source dynamic array.
 * @return true on success, false if allocation fails.
 */
#define da_push_da(da_a, da_b) da_push_arr((da_a), ((da_b)->items), ((da_b)->len))

/**
 * @brief Pops the last element from the array.
 *
 * @param da Pointer to the dynamic array.
 * @param item_out Variable to store the popped element.
 * @return true if an element was popped, false if array is empty.
 */
#define da_pop(da, item_out) da_void_pop((da)->items, &(da)->len, (item_out), DA_ITEM_SIZE(da))

/**
 * @brief Pops the element at a specific index, preserving order.
 *
 * @param da Pointer to the dynamic array.
 * @param index Position to pop at.
 * @param item_out Variable to store the popped element.
 * @return true if an element was popped, false if index is out of range.
 */
#define da_pop_at_ordered(da, index, item_out) da_void_pop_at_ordered((da)->items, &(da)->len, (item_out), DA_ITEM_SIZE(da), (index))

/**
 * @brief Pops the element at a specific index using swap-remove.
 *
 * @param da Pointer to the dynamic array.
 * @param index Position to pop at.
 * @param item_out Variable to store the popped element.
 * @return true if an element was popped, false if index is out of range.
 */
#define da_pop_at_unordered(da, index, item_out) da_void_pop_at_unordered((da)->items, &(da)->len, (item_out), DA_ITEM_SIZE(da), (index))

/**
 * @brief Frees all memory associated with the array.
 *
 * @param da Pointer to the dynamic array.
 */
#define da_free(da) da_void_free(&(da)->items, &(da)->len, &(da)->cap)

/**
 * @brief Sets the length to zero, effectively clearing all elements without freeing memory.
 *
 * @param da Pointer to the dynamic array.
 */
#define da_clear(da) do { (da)->len = 0; } while (0)

/**
 * @brief Clears the destination array and copies all elements from the source.
 *
 * @param dest_da Destination dynamic array (cleared first).
 * @param src_da Source dynamic array.
 */
#define da_copy(dest_da, src_da) (da_clear(dest_da), da_push_da((dest_da), (src_da)))

/**
 * @brief Iterates over all elements in the array with a typed pointer.
 *
 * @param T Element type.
 * @param it Iterator variable name.
 * @param da Pointer to the dynamic array.
 *
 * @par Example:
 * @code
 * da_foreach(int, it, my_array) {
 *     printf("%d\n", *it);
 * }
 * @endcode
 */
#define da_foreach(T, it, da) for (T *it = (da)->items; it < (da)->items + (da)->len; ++it)

/**
 * @brief Iterates over all elements in the array with a const typed pointer.
 *
 * @param T Element type (const).
 * @param it Iterator variable name.
 * @param da Pointer to the dynamic array.
 */
#define da_foreach_const(T, it, da) for (const T *it = (da)->items; it < (da)->items + (da)->len; ++it)

/**
 * @brief Iterates over array indices.
 *
 * @param it Index variable name (size_t).
 * @param da Pointer to the dynamic array.
 *
 * @par Example:
 * @code
 * da_fori(i, my_array) {
 *     printf("%zu: %d\n", i, da_get(my_array, i));
 * }
 * @endcode
 */
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
