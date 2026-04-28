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
 * @file ciao_vista.h
 * @brief Views into string data - no ownership, no allocation.
 *
 * This library provides a String_View type for efficiently representing
 * string slices without memory allocation. Views reference existing string
 * data and track a pointer and length.
 *
 * @par Usage Example:
 * @code
 * #define CIAO_VISTA_IMPLEMENTATION
 * #define CIAO_STRIP_PREFIX
 * #include "ciao_vista.h"
 *
 * const char *text = "  hello world  ";
 * Ciao_String_View sv = ciao_sv_from_zstr(text);
 * sv = ciao_sv_trim(sv);  // "hello world"
 *
 * Ciao_String_View prefix, suffix;
 * ciao_sv_split_by_char(sv, ' ', &prefix, &suffix);  // prefix="hello", suffix="world"
 * @endcode
 */

#ifndef __CIAO_VISTA_H
#define __CIAO_VISTA_H


#include <stddef.h>
#include <stdbool.h>

/**
 * A non-owning view into a string.
 *
 * Contains a pointer to string data and its length. Does not own memory.
 */
typedef struct Ciao_String_View {
  const char *data;  ///< Pointer to the string data.
  size_t len;       ///< Length of the string in bytes.
} Ciao_String_View;

#define Ciao_Vista Ciao_String_View

/**
 * Constructs a Ciao_String_View from a pointer and length.
 *
 * @param ptr Pointer to the string data.
 * @param length Number of bytes.
 * @return A Ciao_String_View with the given data and length.
 */
#define ciao_sv_from_parts(ptr, length) ((Ciao_String_View) { .data = (ptr), .len = (length) })

/**
 * Printf format specifier for Ciao_String_View.
 *
 * Use with CIAO_SV_Arg to print a Ciao_String_View.
 *
 * @par Example:
 * @code
 * printf("Text: " CIAO_SV_Fmt "\n", CIAO_SV_Arg(my_view));
 * @endcode
 */
#define CIAO_SV_Fmt "%.*s"

/**
 * Arguments for printf-style formatting of Ciao_String_View.
 *
 * @param sv A Ciao_String_View variable.
 * @return Arguments suitable for CIAO_SV_Fmt.
 */
#define CIAO_SV_Arg(sv) (int)(sv).len, (sv).data

/**
 * A null/empty Ciao_String_View constant.
 *
 * Equivalent to ciao_sv_from_parts(NULL, 0).
 */
#define CIAO_SV_NULL ((Ciao_String_View) {0, 0})

/**
 * Creates a Ciao_String_View from a null-terminated string.
 *
 * @param zstr Pointer to a null-terminated string (can be NULL).
 * @return A Ciao_String_View covering the entire string.
 */
Ciao_String_View ciao_sv_from_zstr(const char *zstr);

/**
 * Compares two Ciao_String_Views lexicographically.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return -1 if a < b, 0 if equal, 1 if a > b.
 */
int ciao_sv_compare(Ciao_String_View a, Ciao_String_View b);

/**
 * Checks if a Ciao_String_View has zero length.
 *
 * @param sv The string view to check.
 * @return true if len is 0, false otherwise.
 */
bool ciao_sv_is_empty(Ciao_String_View sv);

/**
 * Checks if a Ciao_String_View has a NULL data pointer.
 *
 * @param sv The string view to check.
 * @return true if data is NULL, false otherwise.
 */
bool ciao_sv_is_null(Ciao_String_View sv);

/**
 * Returns a pointer to one past the last character.
 *
 * @param sv The string view.
 * @return Pointer to the end of the string, or NULL if data is NULL.
 */
const char *ciao_sv_end(Ciao_String_View sv);

/**
 * Returns a slice of the string view.
 *
 * @param sv The source string view.
 * @param start Starting index (inclusive).
 * @param end Ending index (exclusive).
 * @return A new Ciao_String_View covering the range [start, end).
 */
Ciao_String_View ciao_sv_slice(Ciao_String_View sv, size_t start, size_t end);

/**
 * Removes n bytes from the left, returning the removed portion.
 *
 * Modifies sv to start after the removed portion.
 *
 * @param[in,out] sv String view to chop from.
 * @param n Number of bytes to remove from left.
 * @return The removed portion.
 */
Ciao_String_View ciao_sv_chop_left(Ciao_String_View *sv, size_t n);

/**
 * Removes n bytes from the right, returning the removed portion.
 *
 * @param[in,out] sv String view to chop from.
 * @param n Number of bytes to remove from right.
 * @return The removed portion.
 */
Ciao_String_View ciao_sv_chop_right(Ciao_String_View *sv, size_t n);

/**
 * Removes and returns the portion before the first delimiter character.
 *
 * @param[in,out] sv String view to chop from.
 * @param delim Delimiter character to search for.
 * @return The portion before delim, or the entire view if delim not found.
 */
Ciao_String_View ciao_sv_chop_by_char(Ciao_String_View *sv, char delim);

/**
 * Removes and returns the portion before the first delimiter string.
 *
 * @param[in,out] sv String view to chop from.
 * @param delim Delimiter string to search for.
 * @return The portion before delim, or the entire view if delim not found.
 */
Ciao_String_View ciao_sv_chop_by_sv(Ciao_String_View *sv, Ciao_String_View delim);

/**
 * Creates a string view from zstr to call sv_chop_by_sv, shorthand.
 *
 * @param[in,out] sv String view to chop from.
 * @param delim Delimiter string to search for, gets turned into a string view.
 * @return The portion before delim, or the entire view if delim not found.
 */
#define ciao_sv_chop_by_zstr(sv, zstr) ciao_sv_chop_by_sv((sv), ciao_sv_from_zstr((zstr)))

/**
 * Removes whitespace from the left side.
 *
 * @param sv The string view to trim.
 * @return A new Ciao_String_View with leading whitespace removed.
 */
Ciao_String_View ciao_sv_trim_left(Ciao_String_View sv);

/**
 * Removes whitespace from the right side.
 *
 * @param sv The string view to trim.
 * @return A new Ciao_String_View with trailing whitespace removed.
 */
Ciao_String_View ciao_sv_trim_right(Ciao_String_View sv);

/**
 * Removes whitespace from both sides.
 *
 * @param sv The string view to trim.
 * @return A new Ciao_String_View with leading and trailing whitespace removed.
 */
Ciao_String_View ciao_sv_trim(Ciao_String_View sv);

/**
 * Finds the first occurrence of a character.
 *
 * @param haystack The string view to search.
 * @param needle The character to find.
 * @param[out] index Pointer to store the position (can be NULL).
 * @return true if found, false otherwise.
 */
bool ciao_sv_find_char(Ciao_String_View haystack, char needle, size_t *index);

/**
 * Finds the first occurrence of a substring.
 *
 * @param haystack The string view to search.
 * @param needle The substring to find.
 * @param[out] index Pointer to store the position (can be NULL).
 * @return true if found, false otherwise.
 */
bool ciao_sv_find_sv(Ciao_String_View haystack, Ciao_String_View needle, size_t *index);

/**
 * Splits a string view at the first delimiter character.
 *
 * @param haystack The string view to split.
 * @param delim Delimiter character.
 * @param[out] left Portion before the delimiter (can be NULL).
 * @param[out] right Portion after the delimiter (can be NULL).
 */
void ciao_sv_split_by_char(Ciao_String_View haystack, char delim, Ciao_String_View *left, Ciao_String_View *right);

/**
 * Splits a string view at the first delimiter string.
 *
 * @param haystack The string view to split.
 * @param delim Delimiter string.
 * @param[out] left Portion before the delimiter (can be NULL).
 * @param[out] right Portion after the delimiter (can be NULL).
 */
void ciao_sv_split_by_sv(Ciao_String_View haystack, Ciao_String_View delim, Ciao_String_View *left, Ciao_String_View *right);

/**
 * Checks if sv starts with the given prefix.
 *
 * @param sv The string view to check.
 * @param prefix The prefix to look for.
 * @return true if sv starts with prefix, false otherwise.
 */
bool ciao_sv_has_sv_prefix(Ciao_String_View sv, Ciao_String_View prefix);

/**
 * Checks if sv ends with the given suffix.
 *
 * @param sv The string view to check.
 * @param suffix The suffix to look for.
 * @return true if sv ends with suffix, false otherwise.
 */
bool ciao_sv_has_sv_suffix(Ciao_String_View sv, Ciao_String_View suffix);

/**
 * Checks if sv starts with a null-terminated prefix.
 *
 * @param sv The string view to check.
 * @param prefix Null-terminated prefix string.
 * @return true if sv starts with prefix, false otherwise.
 */
bool ciao_sv_has_zstr_prefix(Ciao_String_View sv, const char *prefix);

/**
 * Checks if sv ends with a null-terminated suffix.
 *
 * @param sv The string view to check.
 * @param suffix Null-terminated suffix string.
 * @return true if sv ends with suffix, false otherwise.
 */
bool ciao_sv_has_zstr_suffix(Ciao_String_View sv, const char *suffix);

/**
 * Checks if sv starts with the given character.
 *
 * @param sv The string view to check.
 * @param prefix The character to look for.
 * @return true if sv starts with the character, false otherwise.
 */
bool ciao_sv_has_char_prefix(Ciao_String_View sv, char prefix);

/**
 * Checks if sv ends with the given character.
 *
 * @param sv The string view to check.
 * @param suffix The character to look for.
 * @return true if sv ends with the character, false otherwise.
 */
bool ciao_sv_has_char_suffix(Ciao_String_View sv, char suffix);

/**
 * Checks if two Ciao_String_Views are equal.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return true if both views have the same length and data, false otherwise.
 */
bool ciao_sv_eq_sv(Ciao_String_View a, Ciao_String_View b);

/**
 * Checks if two Ciao_String_Views are equal, ignoring ASCII case.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return true if equal ignoring case differences, false otherwise.
 */
bool ciao_sv_eq_sv_ascii_ignore_case(Ciao_String_View a, Ciao_String_View b);

/**
 * Checks if a Ciao_String_View equals a null-terminated string.
 *
 * @param a String view to compare.
 * @param b Null-terminated string.
 * @return true if equal, false otherwise.
 */
bool ciao_sv_eq_zstr(Ciao_String_View a, const char *b);

/**
 * Checks if two null-terminated strings are equal.
 *
 * @param a First null-terminated string.
 * @param b Second null-terminated string.
 * @return true if equal, false otherwise.
 */
bool ciao_zstr_eq(const char *a, const char *b);

/**
 * Checks if a null-terminated string ends with a suffix.
 *
 * @param zstr The null-terminated string to check.
 * @param suffix The suffix to look for.
 * @return true if zstr ends with suffix, false otherwise.
 */
bool ciao_zstr_has_suffix(const char *zstr, const char *suffix);

/**
 * Checks if a null-terminated string starts with a prefix.
 *
 * @param zstr The null-terminated string to check.
 * @param prefix The prefix to look for.
 * @return true if zstr starts with prefix, false otherwise.
 */
bool ciao_zstr_has_prefix(const char *zstr, const char *prefix);

/**
 * Returns the length of a null-terminated string.
 *
 * @param zstr The null-terminated string (can be NULL).
 * @return The length in bytes, or 0 if NULL.
 */
size_t ciao_zstr_len(const char *zstr);

#endif // __CIAO_VISTA_H

#if (defined(CIAO_VISTA_STRIP_PREFIX) || defined(CIAO_STRIP_PREFIX)) && !defined(__CIAO_VISTA_STRIP_PREFIX)
#define __CIAO_VISTA_STRIP_PREFIX

#define String_View Ciao_String_View
#define Vista Ciao_String_View
#define sv_from_parts ciao_sv_from_parts
#define SV_Fmt CIAO_SV_Fmt
#define SV_Arg CIAO_SV_Arg
#define SV_NULL CIAO_SV_NULL
#define sv_from_zstr ciao_sv_from_zstr
#define sv_compare ciao_sv_compare
#define sv_is_empty ciao_sv_is_empty
#define sv_is_null ciao_sv_is_null
#define sv_end ciao_sv_end
#define sv_slice ciao_sv_slice
#define sv_chop_left ciao_sv_chop_left
#define sv_chop_right ciao_sv_chop_right
#define sv_chop_by_char ciao_sv_chop_by_char
#define sv_chop_by_sv ciao_sv_chop_by_sv
#define sv_chop_by_zstr ciao_sv_chop_by_zstr
#define sv_trim_left ciao_sv_trim_left
#define sv_trim_right ciao_sv_trim_right
#define sv_trim ciao_sv_trim
#define sv_find_char ciao_sv_find_char
#define sv_find_sv ciao_sv_find_sv
#define sv_split_by_char ciao_sv_split_by_char
#define sv_split_by_sv ciao_sv_split_by_sv
#define sv_has_sv_prefix ciao_sv_has_sv_prefix
#define sv_has_sv_suffix ciao_sv_has_sv_suffix
#define sv_has_zstr_prefix ciao_sv_has_zstr_prefix
#define sv_has_zstr_suffix ciao_sv_has_zstr_suffix
#define sv_has_char_prefix ciao_sv_has_char_prefix
#define sv_has_char_suffix ciao_sv_has_char_suffix
#define sv_eq_sv ciao_sv_eq_sv
#define sv_eq_sv_ascii_ignore_case ciao_sv_eq_sv_ascii_ignore_case
#define sv_eq_zstr ciao_sv_eq_zstr
#define zstr_eq ciao_zstr_eq
#define zstr_has_suffix ciao_zstr_has_suffix
#define zstr_has_prefix ciao_zstr_has_prefix
#define zstr_len ciao_zstr_len

#endif // CIAO_VISTA_STRIP_PREFIX


#if defined(CIAO_VISTA_IMPLEMENTATION) || defined(CIAO_IMPLEMENTATION)

#define STR_VISTA__ASCII_CASE_DIF ('a' - 'A')

static inline bool ciao__vista_char_is_ascii_whitespace(char c) {
  return (c == ' '  || c == '\n' || c == '\r' ||
  c == '\t' || c == '\v' || c == '\f');
}

Ciao_String_View ciao_sv_from_zstr(const char *zstr) {
  if (zstr == NULL) return CIAO_SV_NULL;
  Ciao_String_View sv = { .data = zstr };
  while (*zstr) {
    sv.len++;
    zstr++;
  }
  return sv;
}


int ciao_sv_compare(Ciao_String_View a, Ciao_String_View b) {
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

bool ciao_sv_is_empty(Ciao_String_View sv) {
  return sv.len == 0;
}

bool ciao_sv_is_null(Ciao_String_View sv) {
  return sv.data == NULL;
}

const char *ciao_sv_end(Ciao_String_View sv) {
  if (sv.data == NULL) return NULL;
  return sv.data + sv.len;
}


Ciao_String_View ciao_sv_slice(Ciao_String_View sv, size_t start, size_t end) {
  if (sv.data == NULL) return CIAO_SV_NULL;
  if (start >= sv.len) return ciao_sv_from_parts(sv.data + sv.len, 0);
  if (end < start) return ciao_sv_from_parts(sv.data + start, 0);
  if (end > sv.len) end = sv.len;
  return ciao_sv_from_parts(sv.data + start, end - start);
}

Ciao_String_View ciao_sv_chop_left(Ciao_String_View *sv, size_t n) {
  if (sv->data == NULL) return CIAO_SV_NULL;
  if (sv->len < n) {
    Ciao_String_View result = *sv;
    sv->data += sv->len;
    sv->len   = 0;
    return result;
  }
  Ciao_String_View result = ciao_sv_from_parts(sv->data, n);
  sv->data += n;
  sv->len  -= n;
  return result;
}

Ciao_String_View ciao_sv_chop_right(Ciao_String_View *sv, size_t n) {
  if (sv->data == NULL) return CIAO_SV_NULL;
  if (sv->len < n) {
    Ciao_String_View result = *sv;
    sv->data += sv->len;
    sv->len = 0;
    return result;
  }
  Ciao_String_View result = ciao_sv_from_parts(sv->data + (sv->len - n), n);
  sv->len  -= n;
  return result;
}

Ciao_String_View ciao_sv_chop_by_char(Ciao_String_View *sv, char delim) {
  Ciao_String_View left;
  ciao_sv_split_by_char(*sv, delim, &left, sv);
  return left;
}

Ciao_String_View ciao_sv_chop_by_sv(Ciao_String_View *sv, Ciao_String_View delim) {
  Ciao_String_View left;
  ciao_sv_split_by_sv(*sv, delim, &left, sv);
  return left;
}

Ciao_String_View ciao_sv_trim_left(Ciao_String_View sv) {
  if (sv.data == NULL) return CIAO_SV_NULL;
  Ciao_String_View result = sv;
  while (result.len && ciao__vista_char_is_ascii_whitespace(*result.data)) {
    result.data++;
    result.len--;
  }
  return result;
}

Ciao_String_View ciao_sv_trim_right(Ciao_String_View sv) {
  if (sv.data == NULL) return CIAO_SV_NULL;
  Ciao_String_View result = sv;
  while (result.len && ciao__vista_char_is_ascii_whitespace(result.data[result.len - 1])) {
    result.len--;
  }
  return result;
}

Ciao_String_View ciao_sv_trim(Ciao_String_View sv) {
  if (sv.data == NULL) return CIAO_SV_NULL;
  return ciao_sv_trim_right(ciao_sv_trim_left(sv));
}

bool ciao_sv_find_char(Ciao_String_View haystack, char needle, size_t *index) {
  if (haystack.data == NULL) return false;
  for (size_t i = 0; i < haystack.len; ++i) {
    if (haystack.data[i] == needle) {
      if (index) *index = i;
      return true;
    }
  }
  return false;
}

bool ciao_sv_find_sv(Ciao_String_View haystack, Ciao_String_View needle, size_t *index) {
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

void ciao_sv_split_by_char(Ciao_String_View haystack, char needle, Ciao_String_View *left, Ciao_String_View *right) {
  if (haystack.data == NULL) return;
  size_t index;
  if (!ciao_sv_find_char(haystack, needle, &index)) {
    if (left) *left = haystack;
    if (right) *right = ciao_sv_from_parts(haystack.data + haystack.len, 0);
    return;
  }
  if (left) *left = ciao_sv_from_parts(haystack.data, index);
  if (right) *right = ciao_sv_from_parts(haystack.data + index + 1, haystack.len - index - 1);
}

void ciao_sv_split_by_sv(Ciao_String_View haystack, Ciao_String_View needle, Ciao_String_View *left, Ciao_String_View *right) {
  if (haystack.data == NULL) return;
  size_t index;
  if (!ciao_sv_find_sv(haystack, needle, &index)) {
    if (left) *left = haystack;
    if (right) *right = ciao_sv_from_parts(haystack.data + haystack.len, 0);
    return;
  }
  if (left) *left = ciao_sv_from_parts(haystack.data, index);
  if (right) *right = ciao_sv_from_parts(haystack.data + index + needle.len, haystack.len - index - needle.len);
}

bool ciao_sv_has_sv_prefix(Ciao_String_View sv, Ciao_String_View prefix) {
  if (sv.data == NULL || prefix.data == NULL) return false;
  if (sv.len < prefix.len) return false;
  for (size_t i = 0; i < prefix.len; ++i) {
    if (sv.data[i] != prefix.data[i]) return false;
  }
  return true;
}

bool ciao_sv_has_sv_suffix(Ciao_String_View sv, Ciao_String_View suffix) {
  if (sv.data == NULL || suffix.data == NULL) return false;
  if (sv.len < suffix.len) return false;
  const char *d = sv.data + (sv.len - suffix.len);
  for (size_t i = 0; i < suffix.len; ++i) {
    if (d[i] != suffix.data[i]) return false;
  }
  return true;
}

bool ciao_sv_has_zstr_prefix(Ciao_String_View sv, const char *prefix) {
  return ciao_sv_has_sv_prefix(sv, ciao_sv_from_zstr(prefix));
}

bool ciao_sv_has_zstr_suffix(Ciao_String_View sv, const char *suffix) {
  return ciao_sv_has_sv_suffix(sv, ciao_sv_from_zstr(suffix));
}


bool ciao_sv_has_char_prefix(Ciao_String_View sv, char prefix) {
  if (sv.data == NULL || sv.len == 0) return false;
  return *sv.data == prefix;
}

bool ciao_sv_has_char_suffix(Ciao_String_View sv, char suffix) {
  if (sv.data == NULL || sv.len == 0) return false;
  return *(sv.data + (sv.len - 1)) == suffix;
}

bool ciao_sv_eq_sv(Ciao_String_View a, Ciao_String_View b) {
  if (a.len != b.len) return false;
  for (size_t i = 0; i < a.len; ++i) {
    if (a.data[i] != b.data[i]) return false;
  }
  return true;
}

bool ciao_sv_eq_sv_ascii_ignore_case(Ciao_String_View a, Ciao_String_View b) {
  if (a.len != b.len) return false;
  for (size_t i = 0; i < a.len; ++i) {
    char ca = a.data[i];
    if ('a' <= ca && ca <= 'z') ca -= STR_VISTA__ASCII_CASE_DIF;
    char cb = b.data[i];
    if ('a' <= cb && cb <= 'z') cb -= STR_VISTA__ASCII_CASE_DIF;
    if (ca != cb) return false;
  }
  return true;
}

bool ciao_sv_eq_zstr(Ciao_String_View a, const char *b) {
  size_t len = 0;
  {
    const char *it = b;
    while (*it != 0) {
      len++;
      it++;
      if (len > a.len) return false;
    }
  }
  if (a.len != len) return false;
  for (size_t i = 0; i < len; ++i) {
    if (a.data[i] != b[i]) return false;
  }
  return true;
}

bool ciao_zstr_eq(const char *a, const char *b) {
  while (*a && *b && *a == *b) {
    a++;
    b++;
  }
  return *a == *b;
}

bool ciao_zstr_has_suffix(const char *zstr, const char *suffix) {
  size_t len = ciao_zstr_len(zstr);
  size_t suf_len = ciao_zstr_len(suffix);
  if (len < suf_len) return false;
  const char *p = zstr + (len - suf_len);
  for (size_t i = 0; i < suf_len; ++i) {
    if (p[i] != suffix[i]) return false;
  }
  return true;
}

bool ciao_zstr_has_prefix(const char *zstr, const char *prefix) {
  while (*zstr && *prefix && *zstr == *prefix) {
    zstr++;
    prefix++;
  }
  return *prefix == '\0';
}

size_t ciao_zstr_len(const char *zstr) {
  if (zstr == NULL) return 0;
  const char *end = zstr;
  while (*end) end++;
  return end - zstr;
}



#endif // CIAO_VISTA_IMPLEMENTATION
