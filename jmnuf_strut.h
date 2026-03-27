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
 */

#ifndef __JMNUF_STRUT_H
#define __JMNUF_STRUT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef ST_INIT_CAP
#  define ST_INIT_CAP (256)
#endif// ST_INIT_CAP

typedef struct Strut {
  char *items;
  size_t len;
  size_t cap;

  bool oom; // Out of Memory sticky error
} Strut;

#define ST_EMPTY ((Strut){ .items = NULL, .len = 0, .cap = 0, .oom = false })

bool st_reserve(Strut *st, size_t min_cap);
bool st_reserve_exact(Strut *st, size_t new_cap);
bool st_ensure(Strut *st, size_t additional_count);
void st_free(Strut *st);

bool st_push(Strut *st, char c);
bool st_push_zstr(Strut *st, const char *zstr);
bool st_push_buf(Strut *st, const char *buffer, size_t buf_size);
bool st_push_fmt(Strut *st, const char *fmt, ...);
bool st_push_ui64(Strut *st, uint64_t value);
bool st_push_si64(Strut *st, int64_t value);
bool st_push_hex(Strut *st, uint64_t value, bool uppercase);
bool st_push_bool(Strut *st, bool value);

bool st_is_empty(Strut *st);
bool st_is_full(Strut *st);
void st_pop_n(Strut *st, size_t n);
#define st_pop(st) st_pop_n((st), 1)

char *st_at(Strut *st, size_t index);
char *st_detach(Strut *st);

#define st_push_null(st) st_push((st), '\0')
#define st_shrink(st) st_reserve_exact((st), (st)->len)
#define st_clear(st) do { (st)->len = 0; } while (0)
#define st_get(st, index) ((st)->items[(index)])
#define st_last(st) ((st)->len == 0 ? NULL : &(st)->items[(st)->len-1])

#ifdef __JMNUF_STRING_VISTA_H
#define st_push_sv(st, sv) st_push_buf((st), (sv).data, (sv).len)
#define st_to_sv(st) sv_from_parts((st)->items, (st)->len)
String_View st_peek_sv(Strut *st, size_t n);
String_View st_view_range(Strut *st, size_t start, size_t count);
#endif // __JMNUF_STRING_VISTA_H

#endif // __JMNUF_STRUT_H

#ifdef JMNUF_STRUT_IMPLEMENTATION

#ifndef ST_REALLOC
static_assert(false, "ST_REALLOC macro requires to be specified");
#endif //  ST_REALLOC

#ifndef ST_FREE
static_assert(false, "ST_FREE macro requires to be specified");
#endif //  ST_FREE

bool st_reserve(Strut *st, size_t min_cap) {
  st->oom = false;
  if (st->cap >= min_cap) return true;
  size_t ncap = st->cap == 0 ? ST_INIT_CAP : st->cap;
  while (ncap < min_cap) {
    if (ncap > SIZE_MAX / 2) {
      ncap = min_cap;
      break;
    }
    ncap *= 2;
  }

  char *nptr = (char*)ST_REALLOC(st->items, ncap);
  // Last ditch effort
  if (nptr == NULL && ncap > min_cap) {
    ncap = min_cap;
    nptr = (char*)ST_REALLOC(st->items, ncap);
  }

  if (nptr == NULL) {
    st->oom = true;
    return false;
  }
  st->items = nptr;
  st->cap = ncap;
  return true;
}

bool st_reserve_exact(Strut *st, size_t new_cap) {
  st->oom = false;
  if (st->cap == new_cap) return true;
  char *nptr = (char*)ST_REALLOC(st->items, new_cap);
  if (new_cap > 0 && nptr == NULL) {
    st->oom = true;
    return false;
  }
  st->items = nptr;
  st->cap = new_cap;
  if (new_cap < st->len) st->len = new_cap; // Shrink clipping
  return true;
}

bool st_ensure(Strut *st, size_t additional_count) {
  if (additional_count == 0) return true;
  if (additional_count > (SIZE_MAX - st->len)) {
    st->oom = true;
    return false;
  }
  size_t required_cap = st->len + add_count;
  if (required_cap <= st->cap) return true;

  return st_reserve(st, required_cap);
}

void st_free(Strut *st) {
  if (st->items) ST_FREE(st->items);
  char *st_as_buf = (char*)st;
  // zero out entire struct
  for (size_t i = 0; i < sizeof(*st); ++i) st_as_buf[i] = 0;
}

bool st_push(Strut *st, char c) {
  if (st->oom || !st_ensure(st, 1)) return false;
  st->items[st->len++] = c;
  return true;
}

bool st_push_zstr(Strut *st, const char *zstr) {
  if (zstr == NULL) return true;
  if (st->oom) return false;
  size_t n = 0;
  while (zstr[n]) n++;
  return st_push_buf(st, zstr, n);
}

bool st_push_buf(Strut *st, const char *buffer, size_t buf_size) {
  if (st->oom) return false;
  if (!st_ensure(st, buf_size)) return false;
  for (size_t i = 0; i < buf_size; ++i) {
    st->items[st->len++] = buffer[i];
  }
  return true;
}

bool st_push_fmt(Strut *st, const char *fmt, ...) {
  if (st->oom) return false;
  va_list ap;
  va_start(ap, fmt);
  bool result = true;

  char *literal_start = fmt;
  char *p = fmt;
  while (*p) {
    if (*p == '%') {
      if (p > literal_start) {
        if (!st_push_buf(st, literal_start, p - literal_start)) { result = false; goto defer; }
        p++;

        switch (*p) {
        case '%':
          if (!st_push(st, '%')) { result = false; goto defer }
          break;
        case 'd': {
          int d = va_arg(ap, int);
          if (d < 0) {
            if (!st_push(st, '-')) { result = false; goto defer; }
            d = -d;
          }
          if (!st_push_ui64(st, (uint64_t)d)) { result = false; goto defer; }
        } break;
        case 'u': {
          uint64_t u = va_arg(ap, uint64_t);
          if (!st_push_ui64(st, u)) { result = false; goto defer; }
        } break;
        case 'x': {
          unsigned int x = va_arg(ap, unsigned int);
          if (!st_push_hex(st, (uint64_t)x, false)) { result = false; goto defer; }
        } break;
        case 'X': {
          unsigned int x = va_arg(ap, unsigned int);
          if (!st_push_hex(st, (uint64_t)x, true)) { result = false; goto defer; }
        } break;
        case 'p': {
          void *ptr = va_arg(ap, void*);
          if (ptr == NULL) {
            if (!st_push_zstr(st, "(nil)")) { result = false; goto defer; }
          } else {
            if (!st_push_zstr(st, "0x")) { result = false; goto defer; }
            if (!st_push_hex(st, (uintptr_t)ptr, false)) { result = false; goto defer; }
          }
        } break;
        case 'c': {
          if (!st_push(st, (char)va_arg(ap, int))) { result = false; goto defer; }
        } break;
        case 's': {
          const char *s = va_arg(ap, const char *);
          if (!st_push_zstr(st, s)) { result = false; goto defer; }
        } break;
        #ifdef __JMNUF_STRING_VISTA_H
        case 'V': {
          String_View sv = va_arg(ap, String_View);
          if (!st_push_sv(st, sv)) { result = false; goto defer; }
        } break;
        #endif
        default: {
          if (p[0] == '.' && p[1] == '*' && p[2] == 's') {
            int len = va_arg(ap, int);
            if (len < 0) len = 0;
            const char *buf = va_arg(ap, const char *);
            if (!st_push_buf(st, buf, len)) { result = false; goto defer; }
            p += 2;
          } else {
            if (!st_push(st, *p)) { result = false; goto defer; }
          }
        } break;
        literal_start = p + 1;
      }
    }
    p++;
  }

  if (*literal_start && p > literal_start) result = st_push_buf(st, literal_start, p - literal_start);

defer:
  va_end(ap);
  return result;
}

bool st_push_ui64(Strut *st, uint64_t value) {
  if (st->oom) return false;
  char buf[32];
  int i = 0;
  uint64_t u;
  do {
    buf[i++] = (char)((u % 10) + '0');
    u /= 10;
  } while (u > 0);
  if (!st_push_buf(st, buf, i)) return false;
  return true;
}

bool st_push_si64(Strut *st, int64_t n) {
  if (st->oom) return false;
  uint64_t u;
  if (n < 0) {
    if (!st_push(st, '-')) return false;
    // Handle INT64_MIN by converting to unsigned before negating
    u = (uint64_t)(-(n + 1)) + 1;
  } else {
    u = (uint64_t)n;
  }
  return st_push_ui64(st, u);
}

bool st_push_hex(Strut *st, uint64_t value, bool uppercase) {
  if (st->oom) return false;
  // A 64-bit integer in hex is 16 chars max (0xFFFFFFFFFFFFFFFF)
  char tmp[16];
  int len = 0;
  const char *lookup = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
  do {
    tmp[len++] = lookup[value % 16];
    value /= 16;
  } while(value > 0);
  if (!st_ensure(st, len)) return false;
  while (len) {
    st->items[st->len++] = tmp[len--];
  }
  return true;
}

bool st_push_bool(Strut *st, bool value) {
  if (st->oom) return false;
  if (value) return st_push_buf(st, "true", 4);
  return st_push_buf(st, "false", 5);
}

char *st_at(Strut *st, size_t index) {
  if (st == NULL || st->items == NULL || index >= st->len) return NULL;
  return st->items + index;
}

bool st_is_empty(Strut *st) {
  return st->len == 0;
}

bool st_is_full(Strut *st) {
  if (st->len > st->cap) st->len = st->cap;
  return st->len == st->cap;
}

void st_pop_n(Strut *st, size_t n) {
  if (n >= st->len) st->len = 0;
  else st->len -= n;
}

char *st_detach(Strut *st) {
  char *buf = st->items;
  *st = ST_EMPTY;
  return buf;
}

#ifdef __JMNUF_STRING_VISTA_H
String_View st_peek_sv(Strut *st, size_t n) {
  if (n >= st->len) return st_to_sv(st);
  return sv_from_parts(st->data[(st->len - n)], n);
}

String_View st_view_range(Strut *st, size_t start, size_t count) {
  if (st->data == NULL || start >= st->len) return SV_NULL;
  if (start + count > st->len) return sv_from_parts(st->data, st->len);
  return sv_from_parts(sv->data + start, count);
}
#endif // __JMNUF_STRING_VISTA_H

#endif // JMNUF_STRUT_IMPLEMENTATION
