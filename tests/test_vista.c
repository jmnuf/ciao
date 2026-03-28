#include "test.h"

#define CIAO_VISTA_IMPLEMENTATION
#define CIAO_STRIP_PREFIX
#include "ciao_vista.h"

#include <ctype.h>

TEST_FN(splitting_by_delimiters) {
  String_View sv = sv_from_zstr("colA,colB,colC,colD\n\tdataAA,dataAB,dataAC\n");
  String_View delim = sv_from_zstr("\n\t");

  String_View left, right;
  sv_split_by_char(sv, ',', &left, &right);
  TEST_CHECK(left.data[left.len-1] != ',' && right.data[0] != ',') {
    log_error("Character delimeter persisted in split when it should not be present");
  }

  TEST_ASSERT(sv_has_sv_prefix(sv, left) && sv.data == left.data);

  sv_split_by_sv(sv, delim, &left, &right);
  TEST_CHECK(!sv_has_sv_suffix(left, delim) && !sv_has_sv_prefix(right, delim)) {
    log_error("SV delimeter persisted in split when it should not be present");
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(trimming_ascii_whitespace) {
  String_View sv = sv_from_zstr("    Hello, World!    ");

  String_View trimmed = sv_trim(sv);
  TEST_CHECK(trimmed.len == 13) {
    log_info("Didn't remove all the whitespace or took more than should have");
    log_info("SV('"SV_Fmt"') -> SV('"SV_Fmt"')", SV_Arg(sv), SV_Arg(trimmed));
  }

  // Handling all whitespace characters that isspace handles
  // Ref: https://en.cppreference.com/w/c/string/byte/isspace
  sv = sv_from_zstr(" \f\n\r\t\vE \v\t\r\n\f");
  trimmed = sv_trim_left(sv);
  TEST_CHECK(trimmed.data[0] == 'E') {
    log_info("Didn't remove all the whitespace or took more than should have");
    log_info("SV("SV_Fmt") -> SV("SV_Fmt")", SV_Arg(sv), SV_Arg(trimmed));
  }
  trimmed = sv_trim_right(sv);
  TEST_CHECK(trimmed.data[trimmed.len-1] == 'E') {
    log_info("Didn't remove all the whitespace or took more than should have");
    log_info("SV("SV_Fmt") -> SV("SV_Fmt")", SV_Arg(sv), SV_Arg(trimmed));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(find_needle_in_haystack) {
  String_View haystack = sv_from_zstr("The quick brown fox jumps over the lazy dog");
  size_t index;

  TEST_CHECK(sv_find_char(haystack, 'z', &index) && index == 37) {
    log_info("Index: %zu", index);
    log_info("Haystack: %s", haystack.data);
    String_View prefix = sv_from_parts(haystack.data, index);
    log_info("Str to index: "SV_Fmt, SV_Arg(prefix));
  }
  TEST_CHECK(sv_find_char(haystack, 'T', &index) && index ==  0) {
    log_info("Index: %zu", index);
    log_info("Haystack: %s", haystack.data);
    String_View prefix = sv_from_parts(haystack.data, index);
    log_info("Str to index: "SV_Fmt, SV_Arg(prefix));
  }
  TEST_CHECK(sv_find_char(haystack, 'g', &index) && index == 42) {
    log_info("Index: %zu", index);
    log_info("Haystack: %s", haystack.data);
    String_View prefix = sv_from_parts(haystack.data, index);
    log_info("Str to index: "SV_Fmt, SV_Arg(prefix));
  }

  String_View needle = sv_from_zstr("lazy");
  TEST_CHECK(sv_find_sv(haystack, needle, &index) && index == 35) {
    log_info("Index: %zu", index);
    log_info("Needle: %s", needle.data);
    log_info("Haystack: %s", haystack.data);
    String_View prefix = sv_from_parts(haystack.data, index);
    log_info("Str to index: "SV_Fmt, SV_Arg(prefix));
  }

  needle = sv_from_zstr("jumps");
  TEST_CHECK(sv_find_sv(haystack, needle, &index) && index == 20) {
    log_info("Index: %zu", index);
    log_info("Needle: %s", needle.data);
    log_info("Haystack: %s", haystack.data);
    String_View prefix = sv_from_parts(haystack.data, index);
    log_info("Str to index: "SV_Fmt, SV_Arg(prefix));
  }

  needle = sv_from_zstr("quick");
  TEST_CHECK(sv_find_sv(haystack, needle, &index) && index == 4) {
    log_info("Index: %zu", index);
    log_info("Needle: %s", needle.data);
    log_info("Haystack: %s", haystack.data);
    String_View prefix = sv_from_parts(haystack.data, index);
    log_info("Str to index: "SV_Fmt, SV_Arg(prefix));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(chopping_string) {
  String_View root = sv_from_zstr("34 35 + call@pn tibet M");
  String_View sv = root;
  String_View chopped;

  char cdelim = ' ';
  chopped = sv_chop_by_char(&sv, cdelim);
  TEST_CHECK(sv_eq_zstr(chopped, "34")) {
    log_info("Chopped: |"SV_Fmt"|", SV_Arg(chopped));
    log_info("Leftover: |"SV_Fmt"|", SV_Arg(sv));
    log_info("Tried to chop by delimeter character(%d): |%c|", (int)cdelim, cdelim);
  }

  int n = 3;
  chopped = sv_chop_left(&sv, n);
  TEST_CHECK(sv_eq_zstr(chopped, "35 ")) {
    log_info("Chopped: |"SV_Fmt"|", SV_Arg(chopped));
    log_info("Leftover: |"SV_Fmt"|", SV_Arg(sv));
    log_info("Tried to chop n(%d) characters from the start of the SV", n);
  }

  n = 2;
  chopped = sv_chop_right(&sv, n);
  TEST_CHECK(sv_eq_zstr(chopped, " M")) {
    log_info("Chopped: |"SV_Fmt"|", SV_Arg(chopped));
    log_info("Leftover: |"SV_Fmt"|", SV_Arg(sv));
    log_info("Tried to chop n(%d) characters from the end of the SV", n);
  }

  String_View sv_delim = sv_from_zstr("call@");
  chopped = sv_chop_by_sv(&sv, sv_delim);
  TEST_CHECK(sv_eq_zstr(chopped, "+ ") && !sv_has_sv_suffix(sv, sv_delim)) {
    log_info("Chopped: |"SV_Fmt"|", SV_Arg(chopped));
    log_info("Leftover: |"SV_Fmt"|", SV_Arg(sv));
    log_info("Tried to chop by sv delimeter: |"SV_Fmt"|", SV_Arg(sv));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(has_suffix) {
  TEST_CHECK(sv_has_char_suffix(SV_NULL, 'H') == false) {}
  TEST_CHECK(sv_has_sv_suffix(SV_NULL, sv_from_parts("foo", 3)) == false) {}
  TEST_CHECK(sv_has_sv_suffix(SV_NULL, SV_NULL) == false) {}
  TEST_CHECK(sv_has_zstr_suffix(SV_NULL, "foo") == false) {}
  TEST_CHECK(sv_has_zstr_suffix(SV_NULL, NULL) == false) {}

  String_View sv = sv_from_zstr("Hello internet, welcome to-");

  TEST_CHECK(sv_has_char_suffix(sv, '-')) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_char_suffix(sv, 'H') == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_sv_suffix(sv, sv_from_zstr("to-"))) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_sv_suffix(sv, sv_from_zstr("theory!")) == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_sv_suffix(sv, SV_NULL) == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_suffix(sv, "welcome to-")) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_suffix(sv, "game theory!") == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_suffix(sv, "")) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_suffix(sv, NULL) == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(has_prefix) {
  TEST_CHECK(sv_has_char_prefix(SV_NULL, 'H') == false) {}
  TEST_CHECK(sv_has_sv_prefix(SV_NULL, sv_from_parts("foo", 3)) == false) {}
  TEST_CHECK(sv_has_sv_prefix(SV_NULL, SV_NULL) == false) {}
  TEST_CHECK(sv_has_zstr_prefix(SV_NULL, "foo") == false) {}
  TEST_CHECK(sv_has_zstr_prefix(SV_NULL, NULL) == false) {}

  String_View sv = sv_from_zstr("Hello internet, welcome to-");

  TEST_CHECK(sv_has_char_prefix(sv, 'H')) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_char_prefix(sv, '-') == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_sv_prefix(sv, sv_from_zstr("Hello"))) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_sv_prefix(sv, sv_from_zstr("hello")) == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_sv_prefix(sv, SV_NULL) == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_prefix(sv, "Hello internet,")) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_prefix(sv, "Hello internet welcome") == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_prefix(sv, "")) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_CHECK(sv_has_zstr_prefix(sv, NULL) == false) {
    log_info("Full string: "SV_Fmt, SV_Arg(sv));
  }

  TEST_EPILOGUE_AND_EXIT();
}

void register_tests(Tests_Registry *registry) {
  Test_Fn_Reg tests[] = {
    TEST_REG(splitting_by_delimiters),
    TEST_REG(trimming_ascii_whitespace),
    TEST_REG(find_needle_in_haystack),
    TEST_REG(chopping_string),
    TEST_REG(has_prefix),
    TEST_REG(has_suffix),
  };
  nob_da_append_many(registry, tests, NOB_ARRAY_LEN(tests));
}

const char *get_tests_title() {
  return "ciao_vista.h : The beach in La Ceiba is a great vista";
}
