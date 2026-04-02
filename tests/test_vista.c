#include "test.h"

#define CIAO_VISTA_IMPLEMENTATION
#include "ciao_vista.h"

#include <ctype.h>

TEST_FN(splitting_by_delimiters) {
  Ciao_String_View sv = ciao_sv_from_zstr("colA,colB,colC,colD\n\tdataAA,dataAB,dataAC\n");
  Ciao_String_View delim = ciao_sv_from_zstr("\n\t");

  Ciao_String_View left, right;
  ciao_sv_split_by_char(sv, ',', &left, &right);
  TEST_CHECK(left.data[left.len-1] != ',' && right.data[0] != ',') {
    log_error("Character delimeter persisted in split when it should not be present");
  }

  TEST_ASSERT(ciao_sv_has_sv_prefix(sv, left) && sv.data == left.data);

  ciao_sv_split_by_sv(sv, delim, &left, &right);
  TEST_CHECK(!ciao_sv_has_sv_suffix(left, delim) && !ciao_sv_has_sv_prefix(right, delim)) {
    log_error("SV delimeter persisted in split when it should not be present");
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(trimming_ascii_whitespace) {
  Ciao_String_View sv = ciao_sv_from_zstr("    Hello, World!    ");

  Ciao_String_View trimmed = ciao_sv_trim(sv);
  TEST_CHECK(trimmed.len == 13) {
    log_info("Didn't remove all the whitespace or took more than should have");
    log_info("SV('"CIAO_SV_Fmt"') -> SV('"CIAO_SV_Fmt"')", CIAO_SV_Arg(sv), CIAO_SV_Arg(trimmed));
  }

  // Handling all whitespace characters that isspace handles
  // Ref: https://en.cppreference.com/w/c/string/byte/isspace
  sv = ciao_sv_from_zstr(" \f\n\r\t\vE \v\t\r\n\f");
  trimmed = ciao_sv_trim_left(sv);
  TEST_CHECK(trimmed.data[0] == 'E') {
    log_info("Didn't remove all the whitespace or took more than should have");
    log_info("SV("CIAO_SV_Fmt") -> SV("CIAO_SV_Fmt")", CIAO_SV_Arg(sv), CIAO_SV_Arg(trimmed));
  }
  trimmed = ciao_sv_trim_right(sv);
  TEST_CHECK(trimmed.data[trimmed.len-1] == 'E') {
    log_info("Didn't remove all the whitespace or took more than should have");
    log_info("SV("CIAO_SV_Fmt") -> SV("CIAO_SV_Fmt")", CIAO_SV_Arg(sv), CIAO_SV_Arg(trimmed));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(find_needle_in_haystack) {
  Ciao_String_View haystack = ciao_sv_from_zstr("The quick brown fox jumps over the lazy dog");
  size_t index;

  TEST_CHECK(ciao_sv_find_char(haystack, 'z', &index) && index == 37) {
    log_info("Index: %zu", index);
    log_info("Haystack: %s", haystack.data);
    Ciao_String_View prefix = ciao_sv_from_parts(haystack.data, index);
    log_info("Str to index: "CIAO_SV_Fmt, CIAO_SV_Arg(prefix));
  }
  TEST_CHECK(ciao_sv_find_char(haystack, 'T', &index) && index ==  0) {
    log_info("Index: %zu", index);
    log_info("Haystack: %s", haystack.data);
    Ciao_String_View prefix = ciao_sv_from_parts(haystack.data, index);
    log_info("Str to index: "CIAO_SV_Fmt, CIAO_SV_Arg(prefix));
  }
  TEST_CHECK(ciao_sv_find_char(haystack, 'g', &index) && index == 42) {
    log_info("Index: %zu", index);
    log_info("Haystack: %s", haystack.data);
    Ciao_String_View prefix = ciao_sv_from_parts(haystack.data, index);
    log_info("Str to index: "CIAO_SV_Fmt, CIAO_SV_Arg(prefix));
  }

  Ciao_String_View needle = ciao_sv_from_zstr("lazy");
  TEST_CHECK(ciao_sv_find_sv(haystack, needle, &index) && index == 35) {
    log_info("Index: %zu", index);
    log_info("Needle: %s", needle.data);
    log_info("Haystack: %s", haystack.data);
    Ciao_String_View prefix = ciao_sv_from_parts(haystack.data, index);
    log_info("Str to index: "CIAO_SV_Fmt, CIAO_SV_Arg(prefix));
  }

  needle = ciao_sv_from_zstr("jumps");
  TEST_CHECK(ciao_sv_find_sv(haystack, needle, &index) && index == 20) {
    log_info("Index: %zu", index);
    log_info("Needle: %s", needle.data);
    log_info("Haystack: %s", haystack.data);
    Ciao_String_View prefix = ciao_sv_from_parts(haystack.data, index);
    log_info("Str to index: "CIAO_SV_Fmt, CIAO_SV_Arg(prefix));
  }

  needle = ciao_sv_from_zstr("quick");
  TEST_CHECK(ciao_sv_find_sv(haystack, needle, &index) && index == 4) {
    log_info("Index: %zu", index);
    log_info("Needle: %s", needle.data);
    log_info("Haystack: %s", haystack.data);
    Ciao_String_View prefix = ciao_sv_from_parts(haystack.data, index);
    log_info("Str to index: "CIAO_SV_Fmt, CIAO_SV_Arg(prefix));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(chopping_string) {
  Ciao_String_View root = ciao_sv_from_zstr("34 35 + call@pn tibet M");
  Ciao_String_View sv = root;
  Ciao_String_View chopped;

  char cdelim = ' ';
  chopped = ciao_sv_chop_by_char(&sv, cdelim);
  TEST_CHECK(ciao_sv_eq_zstr(chopped, "34")) {
    log_info("Chopped: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(chopped));
    log_info("Leftover: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(sv));
    log_info("Tried to chop by delimeter character(%d): |%c|", (int)cdelim, cdelim);
  }

  int n = 3;
  chopped = ciao_sv_chop_left(&sv, n);
  TEST_CHECK(ciao_sv_eq_zstr(chopped, "35 ")) {
    log_info("Chopped: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(chopped));
    log_info("Leftover: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(sv));
    log_info("Tried to chop n(%d) characters from the start of the SV", n);
  }

  n = 2;
  chopped = ciao_sv_chop_right(&sv, n);
  TEST_CHECK(ciao_sv_eq_zstr(chopped, " M")) {
    log_info("Chopped: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(chopped));
    log_info("Leftover: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(sv));
    log_info("Tried to chop n(%d) characters from the end of the SV", n);
  }

  Ciao_String_View sv_delim = ciao_sv_from_zstr("call@");
  chopped = ciao_sv_chop_by_sv(&sv, sv_delim);
  TEST_CHECK(ciao_sv_eq_zstr(chopped, "+ ") && !ciao_sv_has_sv_suffix(sv, sv_delim)) {
    log_info("Chopped: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(chopped));
    log_info("Leftover: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(sv));
    log_info("Tried to chop by sv delimeter: |"CIAO_SV_Fmt"|", CIAO_SV_Arg(sv));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(has_suffix) {
  TEST_CHECK(ciao_sv_has_char_suffix(CIAO_SV_NULL, 'H') == false) {}
  TEST_CHECK(ciao_sv_has_sv_suffix(CIAO_SV_NULL, ciao_sv_from_parts("foo", 3)) == false) {}
  TEST_CHECK(ciao_sv_has_sv_suffix(CIAO_SV_NULL, CIAO_SV_NULL) == false) {}
  TEST_CHECK(ciao_sv_has_zstr_suffix(CIAO_SV_NULL, "foo") == false) {}
  TEST_CHECK(ciao_sv_has_zstr_suffix(CIAO_SV_NULL, NULL) == false) {}

  Ciao_String_View sv = ciao_sv_from_zstr("Hello internet, welcome to-");

  TEST_CHECK(ciao_sv_has_char_suffix(sv, '-')) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_char_suffix(sv, 'H') == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_sv_suffix(sv, ciao_sv_from_zstr("to-"))) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_sv_suffix(sv, ciao_sv_from_zstr("theory!")) == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_sv_suffix(sv, CIAO_SV_NULL) == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_suffix(sv, "welcome to-")) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_suffix(sv, "game theory!") == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_suffix(sv, "")) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_suffix(sv, NULL) == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_EPILOGUE_AND_EXIT();
}

TEST_FN(has_prefix) {
  TEST_CHECK(ciao_sv_has_char_prefix(CIAO_SV_NULL, 'H') == false) {}
  TEST_CHECK(ciao_sv_has_sv_prefix(CIAO_SV_NULL, ciao_sv_from_parts("foo", 3)) == false) {}
  TEST_CHECK(ciao_sv_has_sv_prefix(CIAO_SV_NULL, CIAO_SV_NULL) == false) {}
  TEST_CHECK(ciao_sv_has_zstr_prefix(CIAO_SV_NULL, "foo") == false) {}
  TEST_CHECK(ciao_sv_has_zstr_prefix(CIAO_SV_NULL, NULL) == false) {}

  Ciao_String_View sv = ciao_sv_from_zstr("Hello internet, welcome to-");

  TEST_CHECK(ciao_sv_has_char_prefix(sv, 'H')) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_char_prefix(sv, '-') == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_sv_prefix(sv, ciao_sv_from_zstr("Hello"))) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_sv_prefix(sv, ciao_sv_from_zstr("hello")) == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_sv_prefix(sv, CIAO_SV_NULL) == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_prefix(sv, "Hello internet,")) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_prefix(sv, "Hello internet welcome") == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_prefix(sv, "")) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
  }

  TEST_CHECK(ciao_sv_has_zstr_prefix(sv, NULL) == false) {
    log_info("Full string: "CIAO_SV_Fmt, CIAO_SV_Arg(sv));
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
