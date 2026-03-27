#ifndef __STRING_VIEW_VISTA_H
#define __STRING_VIEW_VISTA_H
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

typedef struct String_View {
  const char *data;
  size_t len;
} String_View;

#define sv_from_parts(ptr, length) ((String_View) { .data = (ptr), .len = (length) })
#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)(sv).len, (sv).data

#define SV_NULL ((String_View) {0, 0})

String_View sv_from_zstr(const char *zstr);

int sv_compare(String_View a, String_View b);

bool is_sv_empty(String_View sv);
bool is_sv_null(String_View sv);
const char *sv_end(String_View sv);

String_View sv_slice(String_View sv, size_t start, size_t end);

String_View sv_chop_left(String_View *sv, size_t n);
String_View sv_chop_right(String_View *sv, size_t n);
String_View sv_chop_by_char(String_View *sv, char delim);
String_View sv_chop_by_sv(String_View *sv, String_View delim);
#define sv_chop_by_zstr(sv, zstr) sv_chop_by_sv((sv), sv_from_zstr((zstr)))

String_View sv_trim_left(String_View sv);
String_View sv_trim_right(String_View sv);
String_View sv_trim(String_View sv);

bool sv_find_char(String_View haystack, char needle, size_t *index);
bool sv_find_sv(String_View haystack, String_View needle, size_t *index);

void sv_split_by_char(String_View haystack, char needle, String_View *left, String_View *right);
void sv_split_by_sv(String_View haystack, String_View needle, String_View *left, String_View *right);

bool sv_has_sv_prefix(String_View sv, String_View prefix);
bool sv_has_sv_suffix(String_View sv, String_View suffix);

bool sv_has_zstr_prefix(String_View sv, const char *prefix);
bool sv_has_zstr_suffix(String_View sv, const char *suffix);

bool sv_has_char_prefix(String_View sv, char prefix);
bool sv_has_char_suffix(String_View sv, char suffix);

bool sv_eq_sv(String_View a, String_View b);
bool sv_eq_sv_ascii_ignore_case(String_View a, String_View b);
bool sv_eq_zstr(String_View a, const char *b);

bool zstr_eq(const char *a, const char *b);
bool zstr_has_suffix(const char *zstr, const char *suffix);
bool zstr_has_prefix(const char *zstr, const char *prefix);
size_t zstr_len(const char *zstr);

#endif // __STRING_VIEW_VISTA_H

#ifdef JMNUF_VISTA_IMPLEMENTATION

#define STRING_VIEW__ASCII_CASE_DIF ('a' - 'A')

static inline bool string_view__is_whitespace(char c) {
  return (c == ' '  || c == '\n' || c == '\r' ||
  c == '\t' || c == '\v' || c == '\f');
}

String_View sv_from_zstr(const char *zstr) {
  if (zstr == NULL) return SV_NULL;
  String_View sv = { .data = zstr };
  while (*zstr) {
    sv.len++;
    zstr++;
  }
  return sv;
}


int sv_compare(String_View a, String_View b) {
  size_t len = a.len <= b.len ? a.len : b.len;
  for (size_t i = 0; i < len; ++i) {
    unsigned char ca = (unsigned char)a.data[i];
    unsigned char cb = (unsigned char)b.data[i];
    if (ca != cb) return ca < cb ? -1 : 1;
  }
  if (a.len < b.len) return -1;
  if (a.len > b.len) return  1;
  return 0;
}

bool is_sv_empty(String_View sv) {
  return sv.len == 0;
}

bool is_sv_null(String_View sv) {
  return sv.data == NULL;
}

const char *sv_end(String_View sv) {
  if (sv.data == NULL) return NULL;
  return sv.data + sv.len;
}


String_View sv_slice(String_View sv, size_t start, size_t end) {
  if (sv->data == NULL) return SV_NULL;
  if (start >= sv.len) return sv_from_parts(sv.data + sv.len, 0);
  if (end < start) return sv_from_parts(sv.data + start, 0);
  if (end > sv.len) end = sv.len;
  return sv_from_parts(sv.data + start, end - start);
}

String_View sv_chop_left(String_View *sv, size_t n) {
  if (sv->data == NULL) return SV_NULL;
  if (sv->len < n) {
    String_View result = *sv;
    sv->data += sv->len;
    sv->len   = 0;
    return result;
  }
  String_View result = sv_from_parts(sv->data, n);
  sv->data += n;
  sv->len  -= n;
  return result;
}

String_View sv_chop_right(String_View *sv, size_t n) {
  if (sv->data == NULL) return SV_NULL;
  if (sv->len < n) {
    String_View result = *sv;
    sv->data += sv->len;
    sv->len = 0;
    return result;
  }
  String_View result = sv_from_parts(sv->data + (sv->len - n), n);
  sv->len  -= n;
  return result;
}

String_View sv_chop_by_char(String_View *sv, char delim) {
  String_View left;
  sv_split_by_char(*sv, delim, &left, sv);
  return left;
}

String_View sv_chop_by_sv(String_View *sv, String_View delim) {
  String_View left;
  sv_split_by_sv(*sv, delim, &left, sv);
  return left;
}

String_View sv_trim_left(String_View sv) {
  if (sv.data == NULL) return SV_NULL;
  String_View result = sv;
  while (result.len && string_view__is_space(*result.data)) {
    result.data++;
    result.len--;
  }
  return result;
}

String_View sv_trim_right(String_View sv) {
  if (sv.data == NULL) return SV_NULL;
  String_View result = sv;
  while (result.len && string_view__is_space(result.data[result.len - 1])) {
    result.len--;
  }
  return result;
}

String_View sv_trim(String_View sv) {
  if (sv.data == NULL) return SV_NULL;
  return sv_trim_right(sv_trim_left(sv));
}

bool sv_find_char(String_View haystack, char needle, size_t *index) {
  if (haystack.data == NULL) return false;
  for (size_t i = 0; i < haystack.len; ++i) {
    if (haystack.data[i] == needle) {
      if (index) *index = i;
      return true;
    }
  }
  return false;
}

bool sv_find_sv(String_View haystack, String_View needle, size_t *index) {
  if (haystack.data == NULL || needle.data == NULL) return false;
  if (needle.len > haystack.len) return false;
  for (size_t i = 0; i <= haystack.len - needle.len; ++i) {
    bool match = true;
    for (size_t j = 0; j < needle.len; ++j) {
      if (haystack.data[i + j] != needle.data[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      if (index) *index = i;
      return true;
    }
  }
  return false;
}

void sv_split_by_char(String_View haystack, char needle, String_View *left, String_View *right) {
  if (haystack.data == NULL) return;
  if (!sv_find_char(haystack, needle, &index)) {
    if (left) *left = haystack;
    if (right) *right = sv_from_parts(haystack.data + haystack.len, 0);
    return;
  }
  if (left) *left = sv_from_parts(haystack.data, index);
  if (right) *right = sv_from_parts(haystack.data + index + 1, haystack.len - index - 1);
}

void sv_split_by_sv(String_View haystack, String_View needle, String_View *left, String_View *right) {
  if (haystack.data == NULL) return;
  size_t index;
  if (!sv_find_sv(haystack, needle, &index)) {
    if (left) *left = haystack;
    if (right) *right = sv_from_parts(haystack.data + haystack.len, 0);
    return;
  }
  if (left) *left = sv_from_parts(haystack.data, index);
  if (right) *right = sv_from_parts(haystack.data + index + needle.len, haystack.len - index - needle.len);
}

bool sv_has_sv_prefix(String_View sv, String_View prefix) {
  if (sv.data == NULL || prefix.data == NULL) return false;
  if (sv.len < prefix.len) return false;
  for (size_t i = 0; i < prefix.len; ++i) {
    if (sv.data[sv.len - (prefix.len - i)] != prefix.data[i]) return false;
  }
  return true;
}

bool sv_has_sv_suffix(String_View sv, String_View suffix) {
  if (sv.data == NULL || suffix.data == NULL) return false;
  if (sv.len < suffix.len) return false;
  for (size_t i = 0; i < suffix.len; ++i) {
    if (sv.data[i] != suffix.data[i]) return false;
  }
  return true;
}

bool sv_has_zstr_prefix(String_View sv, const char *prefix) {
  return sv_has_sv_prefix(sv, sv_from_zstr(prefix));
}

bool sv_has_zstr_suffix(String_View sv, const char *suffix) {
  return sv_has_sv_suffix(sv, sv_from_zstr(suffix));
}


bool sv_has_char_prefix(String_View sv, char prefix) {
  if (sv.data == NULL || sv.len == 0) return false;
  return *sv.data == prefix;
}

bool sv_has_char_suffix(String_View sv, char suffix) {
  if (sv.data == NULL || sv.len == 0) return false;
  return *(sv.data + (sv.len - 1)) == prefix;
}

bool sv_eq_sv(String_View a, String_View b) {
  if (a.len != b.len) return false;
  for (size_t i = 0; i < a.len; ++i) {
    if (a.data[i] != b.data[i]) return false;
  }
  return true;
}

bool sv_eq_sv_ascii_ignore_case(String_View a, String_View b) {
  if (a.len != b.len) return false;
  for (size_t i = 0; i < a.len; ++i) {
    char ca = a.data[i];
    if ('a' <= ca && ca <= 'z') ca -= STRING_VIEW__ASCII_CASE_DIF;
    char cb = b.data[i];
    if ('a' <= cb && cb <= 'z') cb -= STRING_VIEW__ASCII_CASE_DIF;
    if (ca != cb) return false;
  }
  return true;
}

bool sv_eq_zstr(String_View a, const char *b) {
  size_t len = 0;
  {
    const char *it = b;
    while (*it != 0) {
      len++;
      it++;,
      if (len > a.len) return false;
    }
  }
  if (a.len != len) return false;
  for (size_t i = 0; i < len; ++i) {
    if (a.data[i] != b[i]) return false;
  }
  return true;
}

bool zstr_eq(const char *a, const char *b) {
  while (*a && *b && *a == *b) {
    a++;
    b++;
  }
  return *a == *b;
}

bool zstr_has_suffix(const char *zstr, const char *suffix) {
  size_t len = zstr_len(zstr);
  size_t suf_len = zstr_len(suffix);
  if (len < suf_len) return false;
  const char *p = zstr + (len - suf_len);
  for (size_t i = 0; i < suf_len; ++i) {
    if (p[i] != suffix[i]) return false;
  }
  return true;
}

bool zstr_has_prefix(const char *zstr, const char *prefix) {
  while (*zstr && *prefix && *zstr == *prefix) {
    zstr++;
    prefix++;
  }
  return *prefix == '\0';
}

size_t zstr_len(const char *zstr) {
  if (zstr == NULL) return 0;
  const char *end = zstr;
  while (*end) end++;
  return end - zstr;
}



#endif // JMNUF_VISTA_IMPLEMENTATION
