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
 * @file jmnuf_vista.h
 * @brief Views into string data - no ownership, no allocation.
 *
 * This library provides a String_View type for efficiently representing
 * string slices without memory allocation. Views reference existing string
 * data and track a pointer and length.
 *
 * @par Usage Example:
 * @code
 * #define JMNUF_VISTA_IMPLEMENTATION
 * #include "jmnuf_vista.h"
 *
 * const char *text = "  hello world  ";
 * String_View sv = sv_from_zstr(text);
 * sv = sv_trim(sv);  // "hello world"
 *
 * String_View prefix, suffix;
 * sv_split_by_char(sv, ' ', &prefix, &suffix);  // prefix="hello", suffix="world"
 * @endcode
 */

#ifndef __JMNUF_STRING_VISTA_H
#define __JMNUF_STRING_VISTA_H


#include <stddef.h>
#include <stdbool.h>

/**
 * A non-owning view into a string.
 *
 * Contains a pointer to string data and its length. Does not own memory.
 */
typedef struct String_View {
  const char *data;  ///< Pointer to the string data.
  size_t len;       ///< Length of the string in bytes.
} String_View;

/**
 * Constructs a String_View from a pointer and length.
 *
 * @param ptr Pointer to the string data.
 * @param length Number of bytes.
 * @return A String_View with the given data and length.
 */
#define sv_from_parts(ptr, length) ((String_View) { .data = (ptr), .len = (length) })

/**
 * Printf format specifier for String_View.
 *
 * Use with SV_Arg to print a String_View.
 *
 * @par Example:
 * @code
 * printf("Text: " SV_Fmt "\n", SV_Arg(my_view));
 * @endcode
 */
#define SV_Fmt "%.*s"

/**
 * Arguments for printf-style formatting of String_View.
 *
 * @param sv A String_View variable.
 * @return Arguments suitable for SV_Fmt.
 */
#define SV_Arg(sv) (int)(sv).len, (sv).data

/**
 * A null/empty String_View constant.
 *
 * Equivalent to sv_from_parts(NULL, 0).
 */
#define SV_NULL ((String_View) {0, 0})

/**
 * Creates a String_View from a null-terminated string.
 *
 * @param zstr Pointer to a null-terminated string (can be NULL).
 * @return A String_View covering the entire string.
 */
String_View sv_from_zstr(const char *zstr);

/**
 * Compares two String_Views lexicographically.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return -1 if a < b, 0 if equal, 1 if a > b.
 */
int sv_compare(String_View a, String_View b);

/**
 * Checks if a String_View has zero length.
 *
 * @param sv The string view to check.
 * @return true if len is 0, false otherwise.
 */
bool is_sv_empty(String_View sv);

/**
 * Checks if a String_View has a NULL data pointer.
 *
 * @param sv The string view to check.
 * @return true if data is NULL, false otherwise.
 */
bool is_sv_null(String_View sv);

/**
 * Returns a pointer to one past the last character.
 *
 * @param sv The string view.
 * @return Pointer to the end of the string, or NULL if data is NULL.
 */
const char *sv_end(String_View sv);

/**
 * Returns a slice of the string view.
 *
 * @param sv The source string view.
 * @param start Starting index (inclusive).
 * @param end Ending index (exclusive).
 * @return A new String_View covering the range [start, end).
 */
String_View sv_slice(String_View sv, size_t start, size_t end);

/**
 * Removes n bytes from the left, returning the removed portion.
 *
 * Modifies sv to start after the removed portion.
 *
 * @param[in,out] sv String view to chop from.
 * @param n Number of bytes to remove from left.
 * @return The removed portion.
 */
String_View sv_chop_left(String_View *sv, size_t n);

/**
 * Removes n bytes from the right, returning the removed portion.
 *
 * @param[in,out] sv String view to chop from.
 * @param n Number of bytes to remove from right.
 * @return The removed portion.
 */
String_View sv_chop_right(String_View *sv, size_t n);

/**
 * Removes and returns the portion before the first delimiter character.
 *
 * @param[in,out] sv String view to chop from.
 * @param delim Delimiter character to search for.
 * @return The portion before delim, or the entire view if delim not found.
 */
String_View sv_chop_by_char(String_View *sv, char delim);

/**
 * Removes and returns the portion before the first delimiter string.
 *
 * @param[in,out] sv String view to chop from.
 * @param delim Delimiter string to search for.
 * @return The portion before delim, or the entire view if delim not found.
 */
String_View sv_chop_by_sv(String_View *sv, String_View delim);

/**
 * Creates a string view from zstr to call sv_chop_by_sv, shorthand.
 *
 * @param[in,out] sv String view to chop from.
 * @param delim Delimiter string to search for, gets turned into a string view.
 * @return The portion before delim, or the entire view if delim not found.
 */
#define sv_chop_by_zstr(sv, zstr) sv_chop_by_sv((sv), sv_from_zstr((zstr)))

/**
 * Removes whitespace from the left side.
 *
 * @param sv The string view to trim.
 * @return A new String_View with leading whitespace removed.
 */
String_View sv_trim_left(String_View sv);

/**
 * Removes whitespace from the right side.
 *
 * @param sv The string view to trim.
 * @return A new String_View with trailing whitespace removed.
 */
String_View sv_trim_right(String_View sv);

/**
 * Removes whitespace from both sides.
 *
 * @param sv The string view to trim.
 * @return A new String_View with leading and trailing whitespace removed.
 */
String_View sv_trim(String_View sv);

/**
 * Finds the first occurrence of a character.
 *
 * @param haystack The string view to search.
 * @param needle The character to find.
 * @param[out] index Pointer to store the position (can be NULL).
 * @return true if found, false otherwise.
 */
bool sv_find_char(String_View haystack, char needle, size_t *index);

/**
 * Finds the first occurrence of a substring.
 *
 * @param haystack The string view to search.
 * @param needle The substring to find.
 * @param[out] index Pointer to store the position (can be NULL).
 * @return true if found, false otherwise.
 */
bool sv_find_sv(String_View haystack, String_View needle, size_t *index);

/**
 * Splits a string view at the first delimiter character.
 *
 * @param haystack The string view to split.
 * @param delim Delimiter character.
 * @param[out] left Portion before the delimiter (can be NULL).
 * @param[out] right Portion after the delimiter (can be NULL).
 */
void sv_split_by_char(String_View haystack, char delim, String_View *left, String_View *right);

/**
 * Splits a string view at the first delimiter string.
 *
 * @param haystack The string view to split.
 * @param delim Delimiter string.
 * @param[out] left Portion before the delimiter (can be NULL).
 * @param[out] right Portion after the delimiter (can be NULL).
 */
void sv_split_by_sv(String_View haystack, String_View delim, String_View *left, String_View *right);

/**
 * Checks if sv starts with the given prefix.
 *
 * @param sv The string view to check.
 * @param prefix The prefix to look for.
 * @return true if sv starts with prefix, false otherwise.
 */
bool sv_has_sv_prefix(String_View sv, String_View prefix);

/**
 * Checks if sv ends with the given suffix.
 *
 * @param sv The string view to check.
 * @param suffix The suffix to look for.
 * @return true if sv ends with suffix, false otherwise.
 */
bool sv_has_sv_suffix(String_View sv, String_View suffix);

/**
 * Checks if sv starts with a null-terminated prefix.
 *
 * @param sv The string view to check.
 * @param prefix Null-terminated prefix string.
 * @return true if sv starts with prefix, false otherwise.
 */
bool sv_has_zstr_prefix(String_View sv, const char *prefix);

/**
 * Checks if sv ends with a null-terminated suffix.
 *
 * @param sv The string view to check.
 * @param suffix Null-terminated suffix string.
 * @return true if sv ends with suffix, false otherwise.
 */
bool sv_has_zstr_suffix(String_View sv, const char *suffix);

/**
 * Checks if sv starts with the given character.
 *
 * @param sv The string view to check.
 * @param prefix The character to look for.
 * @return true if sv starts with the character, false otherwise.
 */
bool sv_has_char_prefix(String_View sv, char prefix);

/**
 * Checks if sv ends with the given character.
 *
 * @param sv The string view to check.
 * @param suffix The character to look for.
 * @return true if sv ends with the character, false otherwise.
 */
bool sv_has_char_suffix(String_View sv, char suffix);

/**
 * Checks if two String_Views are equal.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return true if both views have the same length and data, false otherwise.
 */
bool sv_eq_sv(String_View a, String_View b);

/**
 * Checks if two String_Views are equal, ignoring ASCII case.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return true if equal ignoring case differences, false otherwise.
 */
bool sv_eq_sv_ascii_ignore_case(String_View a, String_View b);

/**
 * Checks if a String_View equals a null-terminated string.
 *
 * @param a String view to compare.
 * @param b Null-terminated string.
 * @return true if equal, false otherwise.
 */
bool sv_eq_zstr(String_View a, const char *b);

/**
 * Checks if two null-terminated strings are equal.
 *
 * @param a First null-terminated string.
 * @param b Second null-terminated string.
 * @return true if equal, false otherwise.
 */
bool zstr_eq(const char *a, const char *b);

/**
 * Checks if a null-terminated string ends with a suffix.
 *
 * @param zstr The null-terminated string to check.
 * @param suffix The suffix to look for.
 * @return true if zstr ends with suffix, false otherwise.
 */
bool zstr_has_suffix(const char *zstr, const char *suffix);

/**
 * Checks if a null-terminated string starts with a prefix.
 *
 * @param zstr The null-terminated string to check.
 * @param prefix The prefix to look for.
 * @return true if zstr starts with prefix, false otherwise.
 */
bool zstr_has_prefix(const char *zstr, const char *prefix);

/**
 * Returns the length of a null-terminated string.
 *
 * @param zstr The null-terminated string (can be NULL).
 * @return The length in bytes, or 0 if NULL.
 */
size_t zstr_len(const char *zstr);

#endif // __JMNUF_STRING_VISTA_H

#ifdef JMNUF_VISTA_IMPLEMENTATION

#define STR_VISTA__ASCII_CASE_DIF ('a' - 'A')

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
  size_t index;
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
    if (sv.data[i] != prefix.data[i]) return false;
  }
  return true;
}

bool sv_has_sv_suffix(String_View sv, String_View suffix) {
  if (sv.data == NULL || suffix.data == NULL) return false;
  if (sv.len < suffix.len) return false;
  char *d = sv.data + (sv.len - suffix.len);
  for (size_t i = 0; i < suffix.len; ++i) {
    if (d[i] != suffix.data[i]) return false;
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
  return *(sv.data + (sv.len - 1)) == suffix;
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
    if ('a' <= ca && ca <= 'z') ca -= STR_VISTA__ASCII_CASE_DIF;
    char cb = b.data[i];
    if ('a' <= cb && cb <= 'z') cb -= STR_VISTA__ASCII_CASE_DIF;
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
