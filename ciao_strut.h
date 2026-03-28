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
 * @file ciao_strut.h
 * @brief A string builder for strings with presence.
 *
 * @par Usage Example:
 * @code
 * #define CIAO_ST_MEM_REALLOC realloc
 * #define CIAO_ST_MEM_FREE free
 * #define CIAO_STRUT_IMPLEMENTATION
 * #define CIAO_STRIP_PREFIX
 * #include "ciao_strut.h"
 *
 * Strut str = {0};
 * st_push_fmt(&str, "Hello, %s!", "world");
 * st_push_zstr(&str, " Welcome.");
 * st_push_null(&str);
 * printf("%s\n", str.items);  // "Hello, world! Welcome."
 * char *result = st_detach(&str);
 * free(result);
 * @endcode
 */

#ifndef __CIAO_STRUT_H
#define __CIAO_STRUT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef CIAO_ST_INIT_CAP
#  define CIAO_ST_INIT_CAP (256)
#endif// CIAO_ST_INIT_CAP

/**
 * Dynamic string builder struct.
 *
 * - items: Buffer containing the string data.
 * - len: Current string length.
 * - cap: Total buffer capacity.
 * - oom: Out-of-memory sticky error flag.
 */
typedef struct Ciao_Strut {
  char *items;
  size_t len;
  size_t cap;

  bool oom; // Out of Memory sticky error
} Ciao_Strut;

/** Empty/zero-initialized Strut constant. */
#define CIAO_ST_EMPTY ((Ciao_Strut){ .items = NULL, .len = 0, .cap = 0, .oom = false })

/**
 * Ensures minimum capacity, growing exponentially.
 * @param st String builder pointer.
 * @param min_cap Minimum capacity to ensure.
 * @return true on success, false on allocation failure.
 */
bool ciao_st_reserve(Ciao_Strut *st, size_t min_cap);

/**
 * Resizes to exact capacity.
 * @param st String builder pointer.
 * @param new_cap Exact capacity to set.
 * @return true on success, false on allocation failure.
 */
bool ciao_st_reserve_exact(Ciao_Strut *st, size_t new_cap);

/**
 * Ensures space for N additional characters.
 * @param st String builder pointer.
 * @param additional_count Number of extra characters to accommodate.
 * @return true on success, false on allocation failure.
 */
bool ciao_st_ensure(Ciao_Strut *st, size_t additional_count);

/**
 * Frees memory and zeros the struct.
 * @param st String builder pointer.
 */
void ciao_st_free(Ciao_Strut *st);

/**
 * Appends a single character.
 * @param st String builder pointer.
 * @param c Character to append.
 * @return true on success, false if oom or allocation fails.
 */
bool ciao_st_push(Ciao_Strut *st, char c);

/**
 * Appends a null-terminated string.
 * @param st String builder pointer.
 * @param zstr Null-terminated string to append (can be NULL).
 * @return true on success, false if oom or allocation fails.
 */
bool ciao_st_push_zstr(Ciao_Strut *st, const char *zstr);

/**
 * Appends a buffer of given size.
 * @param st String builder pointer.
 * @param buffer Buffer to append.
 * @param buf_size Number of bytes to append.
 * @return true on success, false if allocation fails.
 */
bool ciao_st_push_buf(Ciao_Strut *st, const char *buffer, size_t buf_size);

/**
 * Appends a printf-style formatted string.
 * @param st String builder pointer.
 * @param fmt Format string.
 * @param ... Format arguments.
 * @return true on success, false if oom or allocation fails.
 *
 * Supported specifiers: %d, %u, %x, %X, %p, %c, %s, %.*s
 * @note %V (String_View) only works if ciao_vista.h is included first.
 */
bool ciao_st_push_fmt(Ciao_Strut *st, const char *fmt, ...);

/**
 * Appends an unsigned 64-bit integer.
 * @param st String builder pointer.
 * @param value Integer to append.
 * @return true on success, false if oom or allocation fails.
 */
bool ciao_st_push_ui64(Ciao_Strut *st, uint64_t value);

/**
 * Appends a signed 64-bit integer.
 * @param st String builder pointer.
 * @param value Integer to append.
 * @return true on success, false if oom or allocation fails.
 */
bool ciao_st_push_si64(Ciao_Strut *st, int64_t value);

/**
 * Appends a hex string representation.
 * @param st String builder pointer.
 * @param value Integer to append.
 * @param uppercase Use uppercase hex letters (A-F) if true.
 * @return true on success, false if oom or allocation fails.
 */
bool ciao_st_push_hex(Ciao_Strut *st, uint64_t value, bool uppercase);

/**
 * Appends "true" or "false".
 * @param st String builder pointer.
 * @param value Boolean value.
 * @return true on success, false if oom or allocation fails.
 */
bool ciao_st_push_bool(Ciao_Strut *st, bool value);

/**
 * Returns true if length is 0.
 * @param st String builder pointer.
 * @return true if len is 0, false otherwise.
 */
bool ciao_st_is_empty(Ciao_Strut *st);

/**
 * Returns true if at capacity.
 * @param st String builder pointer.
 * @return true if len equals cap, false otherwise.
 */
bool ciao_st_is_full(Ciao_Strut *st);

/**
 * Removes last N characters.
 * @param st String builder pointer.
 * @param n Number of characters to remove.
 */
void ciao_st_pop_n(Ciao_Strut *st, size_t n);

/** Removes the last character. */
#define ciao_st_pop(st) ciao_st_pop_n((st), 1)

/**
 * Returns pointer to character at index, or NULL if out of bounds.
 * @param st String builder pointer.
 * @param index Character index.
 * @return Pointer to character, or NULL if out of bounds.
 */
char *ciao_st_at(Ciao_Strut *st, size_t index);

/**
 * Returns the buffer and resets the struct.
 * @param st String builder pointer.
 * @return The buffer (caller owns the memory).
 */
char *ciao_st_detach(Ciao_Strut *st);

/** Appends a null character. */
#define ciao_st_push_null(st) ciao_st_push((st), '\0')
/** Shrinks capacity to fit current length. */
#define ciao_st_shrink(st) ciao_st_reserve_exact((st), (st)->len)
/** Sets length to 0 without freeing memory. */
#define ciao_st_clear(st) do { (st)->len = 0; } while (0)
/** Gets character at index. */
#define ciao_st_get(st, index) ((st)->items[(index)])
/** Gets pointer to last character, or NULL if empty. */
#define ciao_st_last(st) ((st)->len == 0 ? NULL : &(st)->items[(st)->len-1])

#ifdef __CIAO_VISTA_H
/** Appends a String_View. */
#define ciao_st_push_sv(st, sv) ciao_st_push_buf((st), (sv).data, (sv).len)
/** Creates a String_View from the current contents. */
#define ciao_st_to_sv(st) ciao_sv_from_parts((st)->items, (st)->len)

/**
 * Returns a String_View of the last N characters.
 * @param st String builder pointer.
 * @param n Number of characters from the end.
 * @return String_View of the last N characters.
 */
String_View ciao_st_peek_sv(Ciao_Strut *st, size_t n);

/**
 * Returns a String_View of a range.
 * @param st String builder pointer.
 * @param start Starting index.
 * @param count Number of characters.
 * @return String_View of the range.
 */
String_View ciao_st_view_range(Ciao_Strut *st, size_t start, size_t count);
#endif // __CIAO_VISTA_H

#ifdef __CIAO_CA_H
Ciao_Strut ciao_st_from_da_void(void **items, size_t *len, size_t *cap, size_t item_size);
#define ciao_st_from_da(da) ciao_st_from_da_void((void**)&(da)->items, &(da)->len, &(da)->cap, DA_ITEM_SIZE(da))
#endif// __CIAO_CA_H

#endif // __CIAO_STRUT_H

#if (defined(CIAO_STRUT_STRIP_PREFIX) || defined(CIAO_STRIP_PREFIX)) && !defined(__CIAO_STRUT_STRIP_PREFIX)
#define __CIAO_STRUT_STRIP_PREFIX

#define Strut Ciao_Strut
#define ST_INIT_CAP CIAO_ST_INIT_CAP
#define ST_EMPTY CIAO_ST_EMPTY
#define st_reserve ciao_st_reserve
#define st_reserve_exact ciao_st_reserve_exact
#define st_ensure ciao_st_ensure
#define st_free ciao_st_free
#define st_push ciao_st_push
#define st_push_zstr ciao_st_push_zstr
#define st_push_buf ciao_st_push_buf
#define st_push_fmt ciao_st_push_fmt
#define st_push_ui64 ciao_st_push_ui64
#define st_push_si64 ciao_st_push_si64
#define st_push_hex ciao_st_push_hex
#define st_push_bool ciao_st_push_bool
#define st_is_empty ciao_st_is_empty
#define st_is_full ciao_st_is_full
#define st_pop_n ciao_st_pop_n
#define st_pop st_pop
#define st_at ciao_st_at
#define st_detach ciao_st_detach
#define st_push_null ciao_st_push_null
#define st_shrink ciao_st_shrink
#define st_clear ciao_st_clear
#define st_get ciao_st_get
#define st_last ciao_st_last
#ifdef __CIAO_VISTA_H
#    define st_push_sv ciao_st_push_sv
#    define st_to_sv ciao_st_to_sv
#    define st_peek_sv ciao_st_peek_sv
#    define st_view_range ciao_st_view_range
#endif // __CIAO_VISTA_H
#ifdef __CIAO_CA_H
#    define st_from_da_void ciao_st_from_da_void
#    define st_from_da ciao_st_from_da
#endif// __CIAO_CA_H
#endif // CIAO_STRUT_STRIP_PREFIX


#ifdef CIAO_STRUT_IMPLEMENTATION

#ifndef CIAO_ST_MEM_REALLOC
static_assert(false, "CIAO_ST_MEM_REALLOC macro requires to be specified");
#endif //  CIAO_ST_MEM_REALLOC

#ifndef CIAO_ST_MEM_FREE
static_assert(false, "CIAO_ST_MEM_FREE macro requires to be specified");
#endif //  CIAO_ST_MEM_FREE

bool ciao_st_reserve(Ciao_Strut *st, size_t min_cap) {
  st->oom = false;
  if (st->cap >= min_cap) return true;
  size_t ncap = st->cap == 0 ? CIAO_ST_INIT_CAP : st->cap;
  while (ncap < min_cap) {
    if (ncap > SIZE_MAX / 2) {
      ncap = min_cap;
      break;
    }
    ncap *= 2;
  }

  char *nptr = (char*)CIAO_ST_MEM_REALLOC(st->items, ncap);
  // Last ditch effort
  if (nptr == NULL && ncap > min_cap) {
    ncap = min_cap;
    nptr = (char*)CIAO_ST_MEM_REALLOC(st->items, ncap);
  }

  if (nptr == NULL) {
    st->oom = true;
    return false;
  }
  st->items = nptr;
  st->cap = ncap;
  return true;
}

bool ciao_st_reserve_exact(Ciao_Strut *st, size_t new_cap) {
  st->oom = false;
  if (st->cap == new_cap) return true;
  char *nptr = (char*)CIAO_ST_MEM_REALLOC(st->items, new_cap);
  if (new_cap > 0 && nptr == NULL) {
    st->oom = true;
    return false;
  }
  st->items = nptr;
  st->cap = new_cap;
  if (new_cap < st->len) st->len = new_cap; // Shrink clipping
  return true;
}

bool ciao_st_ensure(Ciao_Strut *st, size_t additional_count) {
  if (additional_count == 0) return true;
  if (additional_count > (SIZE_MAX - st->len)) {
    st->oom = true;
    return false;
  }
  size_t required_cap = st->len + additional_count;
  if (required_cap <= st->cap) return true;

  return ciao_st_reserve(st, required_cap);
}

void ciao_st_free(Ciao_Strut *st) {
  if (st->items) CIAO_ST_MEM_FREE(st->items);
  char *ciao_st_as_buf = (char*)st;
  // zero out entire struct
  for (size_t i = 0; i < sizeof(*st); ++i) ciao_st_as_buf[i] = 0;
}

bool ciao_st_push(Ciao_Strut *st, char c) {
  if (st->oom || !ciao_st_ensure(st, 1)) return false;
  st->items[st->len++] = c;
  return true;
}

bool ciao_st_push_zstr(Ciao_Strut *st, const char *zstr) {
  if (zstr == NULL) return true;
  if (st->oom) return false;
  size_t n = 0;
  while (zstr[n]) n++;
  return ciao_st_push_buf(st, zstr, n);
}

bool ciao_st_push_buf(Ciao_Strut *st, const char *buffer, size_t buf_size) {
  if (st->oom) return false;
  if (!ciao_st_ensure(st, buf_size)) return false;
  for (size_t i = 0; i < buf_size; ++i) {
    st->items[st->len++] = buffer[i];
  }
  return true;
}

bool ciao_st_push_fmt(Ciao_Strut *st, const char *fmt, ...) {
  if (st->oom) return false;
  va_list ap;
  va_start(ap, fmt);
  bool result = true;

  const char *literal_start = fmt;
  const char *p = fmt;
  while (*p) {
    if (*p == '%') {
      if (p > literal_start) {
        if (!ciao_st_push_buf(st, literal_start, p - literal_start)) { result = false; goto defer; }
      }
      p++;
      switch (*p) {
      case '%':
        if (!ciao_st_push(st, '%')) { result = false; goto defer; }
        break;
        case 'd': {
          int d = va_arg(ap, int);
          if (d < 0) {
            if (!ciao_st_push(st, '-')) { result = false; goto defer; }
            d = -d;
          }
          if (!ciao_st_push_ui64(st, (uint64_t)d)) { result = false; goto defer; }
        } break;
        case 'u': {
          uint64_t u = va_arg(ap, uint64_t);
          if (!ciao_st_push_ui64(st, u)) { result = false; goto defer; }
        } break;
        case 'x': {
          unsigned int x = va_arg(ap, unsigned int);
          if (!ciao_st_push_hex(st, (uint64_t)x, false)) { result = false; goto defer; }
        } break;
        case 'X': {
          unsigned int x = va_arg(ap, unsigned int);
          if (!ciao_st_push_hex(st, (uint64_t)x, true)) { result = false; goto defer; }
        } break;
        case 'p': {
          void *ptr = va_arg(ap, void*);
          if (ptr == NULL) {
            if (!ciao_st_push_zstr(st, "(nil)")) { result = false; goto defer; }
          } else {
            if (!ciao_st_push_zstr(st, "0x")) { result = false; goto defer; }
            if (!ciao_st_push_hex(st, (uintptr_t)ptr, false)) { result = false; goto defer; }
          }
        } break;
        case 'c': {
          if (!ciao_st_push(st, (char)va_arg(ap, int))) { result = false; goto defer; }
        } break;
        case 's': {
          const char *s = va_arg(ap, const char *);
          if (!ciao_st_push_zstr(st, s)) { result = false; goto defer; }
        } break;
        #ifdef __CIAO_VISTA_H
        case 'V': {
          String_View sv = va_arg(ap, String_View);
          if (!ciao_st_push_sv(st, sv)) { result = false; goto defer; }
        } break;
        #endif
        default: {
          if (p[0] == '.' && p[1] == '*' && p[2] == 's') {
            int len = va_arg(ap, int);
            if (len < 0) len = 0;
            const char *buf = va_arg(ap, const char *);
            if (!ciao_st_push_buf(st, buf, len)) { result = false; goto defer; }
            p += 2;
          } else {
            if (!ciao_st_push(st, *p)) { result = false; goto defer; }
          }
        } break;
      }
      literal_start = p + 1;
    }
    p++;
  }

  if (*literal_start && p > literal_start) result = ciao_st_push_buf(st, literal_start, p - literal_start);

defer:
  va_end(ap);
  return result;
}

bool ciao_st_push_ui64(Ciao_Strut *st, uint64_t value) {
  if (st->oom) return false;
  if (value == 0) return ciao_st_push(st, '0');
  char buf[32];
  int i = 0;
  uint64_t u = value;
  do {
    buf[i++] = (char)((u % 10) + '0');
    u /= 10;
  } while (u > 0);
  if (!ciao_st_ensure(st, i)) return false;
  while (i) st->items[st->len++] = buf[--i];
  return true;
}

bool ciao_st_push_si64(Ciao_Strut *st, int64_t n) {
  if (st->oom) return false;
  if (n == 0) return ciao_st_push(st, '0');
  uint64_t u;
  if (n < 0) {
    if (!ciao_st_push(st, '-')) return false;
    // Handle INT64_MIN by converting to unsigned before negating
    u = (uint64_t)(-(n + 1)) + 1;
  } else {
    u = (uint64_t)n;
  }
  return ciao_st_push_ui64(st, u);
}

bool ciao_st_push_hex(Ciao_Strut *st, uint64_t value, bool uppercase) {
  if (st->oom) return false;
  if (value == 0) return ciao_st_push(st, '0');
  // A 64-bit integer in hex is 16 chars max (0xFFFFFFFFFFFFFFFF)
  char tmp[16];
  int len = 0;
  const char *lookup = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
  do {
    tmp[len++] = lookup[value % 16];
    value /= 16;
  } while(value > 0);
  if (!ciao_st_ensure(st, len)) return false;
  while (len) st->items[st->len++] = tmp[--len];
  return true;
}

bool ciao_st_push_bool(Ciao_Strut *st, bool value) {
  if (st->oom) return false;
  if (value) return ciao_st_push_buf(st, "true", 4);
  return ciao_st_push_buf(st, "false", 5);
}

char *ciao_st_at(Ciao_Strut *st, size_t index) {
  if (st == NULL || st->items == NULL || index >= st->len) return NULL;
  return st->items + index;
}

bool ciao_st_is_empty(Ciao_Strut *st) {
  return st->len == 0;
}

bool ciao_st_is_full(Ciao_Strut *st) {
  if (st->len > st->cap) st->len = st->cap;
  return st->len == st->cap;
}

void ciao_st_pop_n(Ciao_Strut *st, size_t n) {
  if (n >= st->len) st->len = 0;
  else st->len -= n;
}

char *ciao_st_detach(Ciao_Strut *st) {
  char *buf = st->items;
  *st = CIAO_ST_EMPTY;
  return buf;
}

#ifdef __CIAO_VISTA_H
String_View ciao_st_peek_sv(Ciao_Strut *st, size_t n) {
  if (n >= st->len) return ciao_st_to_sv(st);
  return sv_from_parts(st->items[(st->len - n)], n);
}

String_View ciao_st_view_range(Ciao_Strut *st, size_t start, size_t count) {
  if (st->data == NULL || start >= st->len) return SV_NULL;
  if (start + count > st->len) return sv_from_parts(st->items, st->len);
  return sv_from_parts(sv->items + start, count);
}
#endif // __CIAO_VISTA_H

#ifdef __CIAO_CA_H

Ciao_Strut ciao_st_from_da_void(void **items, size_t *len, size_t *cap, size_t item_size) {
  Ciao_Strut st = {
    .items = (char*)*items,
    .len   = (*len) * item_size,
    .cap   = (*cap) * item_size,
  };
  *items = NULL;
  *len = *cap = 0;
  return st;
}

#endif// __CIAO_CA_H

#endif // CIAO_STRUT_IMPLEMENTATION
