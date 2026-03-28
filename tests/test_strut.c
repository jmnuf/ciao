#include "test.h"

#define CIAO_ST_MEM_REALLOC realloc
#define CIAO_ST_MEM_FREE free
#define CIAO_STRUT_IMPLEMENTATION
#define CIAO_STRIP_PREFIX
#include "ciao_strut.h"

TEST_FN(st_push) {
  Strut st = {0};
  
  TEST_ASSERT(st_push(&st, 'a'));
  TEST_ASSERT(st.len == 1);
  TEST_ASSERT(st.items[0] == 'a');
  
  TEST_ASSERT(st_push(&st, 'b'));
  TEST_ASSERT(st.len == 2);

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_push_zstr) {
  Strut st = {0};
  
  TEST_ASSERT(st_push_zstr(&st, "hello"));
  TEST_ASSERT(st.len == 5);
  TEST_ASSERT(st.items[0] == 'h');
  TEST_ASSERT(st.items[4] == 'o');
  
  TEST_ASSERT(st_push_zstr(&st, " world"));
  TEST_ASSERT(st.len == 11);
  
  TEST_ASSERT(st_push_zstr(&st, NULL));
  TEST_ASSERT(st.len == 11);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_push_buf) {
  Strut st = {0};
  const char *buf = "test buffer";
  
  TEST_ASSERT(st_push_buf(&st, buf, 4));
  TEST_ASSERT(st.len == 4);
  TEST_ASSERT(st.items[0] == buf[0]);
  TEST_ASSERT(st.items[3] == buf[3]);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

// %d, %u, %x, %X, %p, %c, %s, %.*s
TEST_FN(st_push_fmt) {
  Strut st = {0};
  TEST_ASSERT(st_reserve_exact(&st, CIAO_ST_INIT_CAP));

  st.len = 0;
  st_push_fmt(&st, "%d", 42);
  TEST_CHECK(st.len == 2 && strncmp(st.items, "42", 2) == 0) {
    log_info("St contents: |%.*s|", (int)st.len, st.items);
    log_info("St len: %zu", st.len);
  }
  
  st.len = 0;
  st_push_fmt(&st, "%d", -42);
  TEST_CHECK(st.len == 3 && strncmp(st.items, "-42", 3) == 0) {
    log_info("St contents: |%.*s|", (int)st.len, st.items);
    log_info("St len: %zu", st.len);
  }

  st.len = 0;
  st_push_fmt(&st, "%u", 18446744073709551615ULL);
  TEST_CHECK(st.len == 20 && strncmp(st.items, "18446744073709551615", 20) == 0) {
    log_info("St contents: |%.*s|", (int)st.len, st.items);
    log_info("St len: %zu", st.len);
  }

  st.len = 0;
  st_push_fmt(&st, "%x", 0xFAD);
  TEST_CHECK(st.len == 3 && strncmp(st.items, "fad", 3) == 0) {
    log_info("St contents: |%.*s|", (int)st.len, st.items);
    log_info("St len: %zu", st.len);
  }

  st.len = 0;
  st_push_fmt(&st, "%X", 0xEA);
  TEST_CHECK(st.len == 2 && strncmp(st.items, "EA", 2) == 0) {
    log_info("St contents: |%.*s|", (int)st.len, st.items);
    log_info("St len: %zu", st.len);
  }

  {
    st.len = 0;
    st_push_fmt(&st, "%p", &st);
    int n = snprintf(NULL, 0, "%p", &st);
    TEST_ASSERT(n > 0);
    size_t buf_size = (size_t)(n + 1);
    char *buf = (char*)malloc(buf_size);
    snprintf(buf, buf_size, "%p", &st);
    TEST_CHECK(st.len == (buf_size-1) && strncmp(st.items, buf, n) == 0) {
      log_info("St contents: |%.*s|", (int)st.len, st.items);
      log_info("St len: %zu", st.len);
      log_info("Buffer: |%.*s|", n, buf);
      log_info("Buf len: %zu", buf_size);
    }
  }

  st.len = 0;
  st_push_fmt(&st, "%c", 'E');
  TEST_CHECK(st.len == 1 && st_get(&st, 0) == 'E') {
    log_info("St contents: |%.*s|", (int)st.len, st.items);
    log_info("St len: %zu", st.len);
  }

  {
    st.len = 0;
    Nob_String_View s = nob_sv_from_cstr("Hello, World");
    st_push_fmt(&st, "%s", s.data);
    TEST_CHECK(st.len == s.count && strncmp(st.items, s.data, s.count) == 0) {
      log_info("St contents: |%.*s|", (int)st.len, st.items);
      log_info("St len: %zu", st.len);
      log_info("String: |%s|", s.data);
    }

    st.len = 0;
    Nob_String_View sv = nob_sv_chop_by_delim(&s, ',');
    st_push_fmt(&st, "%.*s", (int)sv.count, sv.data);
    TEST_CHECK(st.len == sv.count && strncmp(st.items, sv.data, sv.count) == 0) {
      log_info("St contents: |%.*s|", (int)st.len, st.items);
      log_info("St len: %zu", st.len);
      log_info("String: |%.*s|", (int)sv.count, sv.data);
    }
  }

  TEST_EPILOGUE();
  TEST_EXIT();
}

TEST_FN(st_push_si64) {
  Strut st = {0};
  
  TEST_ASSERT(st_push_si64(&st, 0));
  TEST_ASSERT(st.len == 1);
  TEST_ASSERT(st.items[0] == '0');

  st.len = 0;
  TEST_ASSERT(st_push_si64(&st, -12345));
  TEST_ASSERT(st.items[0] == '-');
  TEST_ASSERT(st.len == 6);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_push_si64_min) {
  Strut st = {0};
  
  int64_t val = INT64_MIN;
  TEST_ASSERT(st_push_si64(&st, val));
  TEST_ASSERT(st.items[0] == '-');
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_push_ui64) {
  Strut st = {0};
  
  TEST_ASSERT(st_push_ui64(&st, 0));
  TEST_ASSERT(st.items[0] == '0');
  
  st.len = 0;
  TEST_ASSERT(st_push_ui64(&st, 18446744073709551615ULL));
  TEST_ASSERT(st.len == 20);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_push_hex) {
  Strut st = {0};
  
  TEST_ASSERT(st_push_hex(&st, 0, false));
  TEST_ASSERT(st.items[0] == '0');
  
  st.len = 0;
  TEST_ASSERT(st_push_hex(&st, 255, false));
  TEST_ASSERT(st.items[0] == 'f');

  st.len = 0;
  TEST_ASSERT(st_push_hex(&st, 255, true));
  TEST_ASSERT(st.items[0] == 'F');

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_push_bool) {
  Strut st = {0};
  
  TEST_ASSERT(st_push_bool(&st, true));
  TEST_ASSERT(st.len == 4);
  TEST_ASSERT(strncmp(st.items, "true", 4) == 0);
  
  
  st.len = 0;
  TEST_ASSERT(st_push_bool(&st, false));
  TEST_ASSERT(st.len == 5);
  TEST_ASSERT(strncmp(st.items, "false", 5) == 0);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_reserve) {
  Strut st = {0};
  
  TEST_ASSERT(st_reserve(&st, 100));
  TEST_ASSERT(st.cap >= 100);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_ensure) {
  Strut st = {0};
  
  TEST_ASSERT(st_ensure(&st, 50));
  TEST_ASSERT(st.cap >= 50);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_reserve_exact) {
  Strut st = {0};
  st_push_zstr(&st, "hello");
  
  TEST_ASSERT(st_reserve_exact(&st, 10));
  TEST_ASSERT(st.cap == 10);
  TEST_ASSERT(st.len == 5);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_detach) {
  Strut st = {0};
  st_push_zstr(&st, "hello");
  
  char *buf = st_detach(&st);
  TEST_ASSERT(buf != NULL);
  TEST_ASSERT(strcmp(buf, "hello") == 0);
  TEST_ASSERT(st.items == NULL);
  TEST_ASSERT(st.len == 0);
  

  TEST_EPILOGUE();
  if (buf) free(buf);
  TEST_EXIT();
}

TEST_FN(st_shrink) {
  Strut st = {0};
  st_push_zstr(&st, "hello");
  
  TEST_ASSERT(st_shrink(&st));
  TEST_ASSERT(st.cap == st.len);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}

TEST_FN(st_pop_n) {
  Strut st = {0};
  st_push_zstr(&st, "hello");
  
  st_pop_n(&st, 2);
  TEST_ASSERT(st.len == 3);
  TEST_ASSERT(st.items[0] == 'h');
  TEST_ASSERT(st.items[2] == 'l');
  
  st_pop_n(&st, 10);
  TEST_ASSERT(st.len == 0);
  

  TEST_EPILOGUE();
  st_free(&st);
  TEST_EXIT();
}


void register_tests(Tests_Registry *registry) {
  Test_Fn_Reg tests[] = {
    TEST_REG(st_push),
    TEST_REG(st_push_zstr),
    TEST_REG(st_push_buf),
    TEST_REG(st_push_fmt),
    TEST_REG(st_push_si64),
    TEST_REG(st_push_si64_min),
    TEST_REG(st_push_ui64),
    TEST_REG(st_push_hex),
    TEST_REG(st_push_bool),
    TEST_REG(st_reserve),
    TEST_REG(st_ensure),
    TEST_REG(st_reserve_exact),
    TEST_REG(st_detach),
    TEST_REG(st_shrink),
    TEST_REG(st_pop_n),
  };

  nob_da_append_many(registry, tests, NOB_ARRAY_LEN(tests));
}

const char *get_tests_title() {
  return "ciao_strut.h : Strut down that memory gurl";
}

