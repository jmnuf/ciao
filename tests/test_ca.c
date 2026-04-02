#include "test.h"

#define CIAO_CAH_INIT_CAP 16
#define CIAO_CAH_STDLIB_MEM
#define CIAO_CAH_IMPLEMENTATION
#include "ciao_ca.h"

struct Foo { int a, b; };

TEST_FN(basic_push_and_pop) {
  CIAO_DynamicArray(int) xs = {0};
  CIAO_DynamicArray(struct Foo) foos = {0};

  TEST_CHECK(ciao_da_push(&xs, 34)) {
    log_info("Test failure for lack of RAM. Brother how?");
  }

  TEST_CHECK(xs.cap == CIAO_CAH_INIT_CAP) {
    log_warning("First allocation did not allocate CAH_INIT_CAP (%d) space", CIAO_CAH_INIT_CAP);
  }

  TEST_CHECK(xs.len == 1 && xs.items[0] == 34) {
    log_error("First pushed item has incorrect value");
  }

  TEST_CHECK(ciao_da_push(&xs, 35) && xs.len == 2 && xs.cap == CIAO_CAH_INIT_CAP) {
    log_error("Second push should just increment len");
  }

  TEST_CHECK(xs.items[1] == 35) {
    log_error("Second pushed item has incorrect value");
  }

  int x;
  TEST_CHECK(ciao_da_pop(&xs, &x)) {
    log_error("Pop failed with %zu items in it", xs.len);
  }
  TEST_CHECK(x == 35) {
    log_error("Popped value is incorrect");
  }

  TEST_CHECK(ciao_da_pop(&xs, &x)) {
    log_error("Pop failed with %zu items in it", xs.len);
  }
  TEST_CHECK(x == 34) {
    log_error("Popped value is incorrect");
  }

  int arr[] = { 10, 15, 20 };
  size_t arr_len = NOB_ARRAY_LEN(arr);
  size_t prev_len = xs.len;

  ciao_da_push_arr(&xs, arr, arr_len);
  TEST_CHECK(xs.len == prev_len + arr_len) {
    log_error("Array values were not pushed");
    log_info("DArray len: %zu", xs.len);
    log_info("Array len: %zu", arr_len);
  }

  for (size_t i = 0; i < arr_len; ++i) {
    TEST_CHECK(ciao_da_get(&xs, prev_len + i) == arr[i]) {
      log_info("Got value: %d", ciao_da_get(&xs, prev_len + i));
      log_info("Expected value: %d", arr[i]);
      log_info("Index: %zu", i);
    }
  }

  TEST_ASSERT(ciao_da_push(&foos, ((struct Foo){ .a = 2, .b = 4 })));
  TEST_ASSERT(ciao_da_get(&foos, 0).a == 2 && ciao_da_get(&foos, 0).b == 4);

  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}


bool int_find_predicate(const void *item, const void *needle) {
  int x = *(const int*)item;
  int y = *(const int*)needle;
  return x == y;
}

TEST_FN(accessor_and_find) {
  CIAO_DynamicArray(int) xs = {0};

  TEST_CHECK(ciao_da_last(&xs) == NULL) {
    log_error("Returning invalid pointer when empty");
    log_info("    Array length: %zu", xs.len);
    log_info("    Array capacity: %zu", xs.cap);
  }

  ciao_da_push(&xs, 34);
  ciao_da_push(&xs, 35);
  ciao_da_push(&xs, 420);
  ciao_da_push(&xs, 80085);

  TEST_CHECK(ciao_da_last(&xs) == &xs.items[xs.len-1]) {
    log_error("Invalid pointer provided");
    log_info("Array length: %zu", xs.len);
  }

  size_t index, expected_index = 3;
  TEST_CHECK(ciao_da_find(&xs, int_find_predicate, &index, &(int){ciao_da_get(&xs, expected_index)})) {
    log_error("Failed to find item that is inside the dynamic array");
    log_info("Array length: %zu", xs.len);
    log_info("Expected index: %zu", expected_index);
  }

  TEST_CHECK(index == expected_index) {
    log_info("Got: %zu", index);
    log_info("Expected: %zu", expected_index);
  }

  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

TEST_FN(reserving_memory) {
  CIAO_DynamicArray(int) xs = {0};

  TEST_CHECK(ciao_da_reserve_exact(&xs, 4) && xs.cap == 4) {
    log_info("Expected to allocation to have an exact capacity of 4");
    log_info("Actual cap: %zu", xs.cap);
    log_info("items pointer: %p", xs.items);
    if (xs.items == NULL) log_error("You can't be serious");
  }

  for (int i = 0; i < 12; ++i) ciao_da_push(&xs, i);

  TEST_CHECK(xs.cap == 16) {
    log_info("Capacity should be doubling when len is about to exceed it");
    log_info("Actual cap: %zu", xs.cap);
    log_info("Actual len: %zu", xs.len);
    log_info("items pointer: %p", xs.items);
    if (xs.items == NULL) log_error("You can't be serious");
  }

  size_t len = xs.len;
  TEST_CHECK(ciao_da_shrink(&xs) && xs.len == xs.cap && xs.cap == len) {
    log_info("Capacity should be %zu which should be the length of the array", len);
    log_info("Actual cap: %zu", xs.cap);
    log_info("Actual len: %zu", xs.len);
    log_info("items pointer: %p", xs.items);
    if (xs.items == NULL) log_error("You can't be serious");
  }

  ciao_da_free(&xs);
  char *buf = nob_temp_alloc(sizeof(xs));
  memset(buf, 0, sizeof(xs));
  TEST_CHECK(memcmp(&xs, buf, sizeof(xs)) == 0) {
    log_info("Dynamic array should be zeroed out completely when freed");
  }
  
  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

TEST_FN(inserting_at_extremes) {
  CIAO_DynamicArray(int) xs = {0};
  int base[] = { 1, 2, 4, 8, 16 };
  size_t base_len = NOB_ARRAY_LEN(base);

  ciao_da_push_arr(&xs, (int*)base + 1, base_len-2);

  TEST_ASSERT(ciao_da_insert(&xs, base[0], 0));

  TEST_CHECK(xs.len == base_len - 1) {
    log_info("DArray length: %zu", xs.len);
    log_info("base_len - 1: %zu", base_len - 1);
    TEST_QUIT_EARLY();
  }

  ciao_da_fori(i, &xs) {
    TEST_CHECK(ciao_da_get(&xs, i) == base[i]) {
      log_info("Index: %zu", i);
      log_info("Got value: %d", ciao_da_get(&xs, i));
      log_info("Expected value: %d", base[i]);
    }
  }

  TEST_ASSERT(ciao_da_insert(&xs, base[base_len-1], xs.len));
  TEST_CHECK(xs.len == base_len) {
    log_info("DArray length: %zu", xs.len);
    log_info("base_len: %zu", base_len);
    TEST_QUIT_EARLY();
  }

  ciao_da_fori(i, &xs) {
    TEST_CHECK(ciao_da_get(&xs, i) == base[i]) {
      log_info("Index: %zu", i);
      log_info("Got value: %d", ciao_da_get(&xs, i));
      log_info("Expected value: %d", base[i]);
    }
  }

  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

TEST_FN(insert_in_middle) {
  CIAO_DynamicArray(int) xs = {0};
  for (int i = 0; i < 12; ++i) TEST_ASSERT(ciao_da_push(&xs, 20 + i));
  int *arr_mirror = nob_temp_alloc(sizeof(*arr_mirror)*(xs.len+3));

  memcpy(arr_mirror, xs.items, sizeof(*arr_mirror)*xs.len);

  TEST_CHECK(ciao_da_insert(&xs, 2, 4) && ciao_da_get(&xs, 4) == 2) {
    log_info("Value at index 4: %d", ciao_da_get(&xs, 4));
  }

  for (size_t i = 0; i < 4; ++i) {
    TEST_CHECK(ciao_da_get(&xs, i) == arr_mirror[i]) {
      log_info("Index: %zu", i);
      log_info("Value in da: %d", ciao_da_get(&xs, i));
      log_info("Value in arr: %d", arr_mirror[i]);
    }
  }
  for (size_t i = 5; i < xs.len; ++i) {
    TEST_CHECK(ciao_da_get(&xs, i) == arr_mirror[(i - 1)]) {
      log_info("Index: %zu", i);
      log_info("Value in da: %d", ciao_da_get(&xs, i));
      log_info("Value in arr: %d", arr_mirror[(i - 1)]);
    }
  }
  if (test.failed) TEST_QUIT_EARLY();
  memcpy(arr_mirror, xs.items, sizeof(*arr_mirror)*xs.len);

  TEST_CHECK(ciao_da_insert(&xs, 50, 6) && ciao_da_get(&xs, 6) == 50) {
    log_info("Value at index 6: %d", ciao_da_get(&xs, 6));
  }

  for (size_t i = 0; i < 6; ++i) {
    TEST_CHECK(ciao_da_get(&xs, i) == arr_mirror[i]) {
      log_info("Index: %zu", i);
      log_info("Value in da: %d", ciao_da_get(&xs, i));
      log_info("Value in arr: %d", arr_mirror[i]);
    }
  }
  for (size_t i = 7; i < xs.len; ++i) {
    TEST_CHECK(ciao_da_get(&xs, i) == arr_mirror[(i - 1)]) {
      log_info("Index: %zu", i);
      log_info("Value in da: %d", ciao_da_get(&xs, i));
      log_info("Value in arr: %d", arr_mirror[(i - 1)]);
    }
  }
  if (test.failed) TEST_QUIT_EARLY();
  memcpy(arr_mirror, xs.items, sizeof(*arr_mirror)*xs.len);

  TEST_CHECK(ciao_da_insert(&xs, 44, 12) && ciao_da_get(&xs, 12) == 44) {
    log_info("Value at index 12: %d", ciao_da_get(&xs, 12));
  }

  for (size_t i = 0; i < 12; ++i) {
    TEST_CHECK(ciao_da_get(&xs, i) == arr_mirror[i]) {
      log_info("Index: %zu", i);
      log_info("Value in da: %d", ciao_da_get(&xs, i));
      log_info("Value in arr: %d", arr_mirror[i]);
    }
  }
  for (size_t i = 13; i < xs.len; ++i) {
    TEST_CHECK(ciao_da_get(&xs, i) == arr_mirror[(i - 1)]) {
      log_info("Index: %zu", i);
      log_info("Value in da: %d", ciao_da_get(&xs, i));
      log_info("Value in arr: %d", arr_mirror[(i - 1)]);
    }
  }

  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

TEST_FN(insert_out_of_bounds) {
  CIAO_DynamicArray(int) xs = {0};

  TEST_CHECK(ciao_da_insert(&xs, 1, 3) == false) {
    log_error("Should not be allowed to insert at index greater than da len");
    log_info("DArray len: %zu", xs.len);
  }

  TEST_ASSERT(ciao_da_push(&xs, 1));
  TEST_ASSERT(ciao_da_push(&xs, 2));
  TEST_ASSERT(ciao_da_push(&xs, 3));

  TEST_CHECK(ciao_da_insert(&xs, 1, 4) == false) {
    log_error("Should not be allowed to insert at index greater than da len");
    log_info("DArray len: %zu", xs.len);
  }

  TEST_ASSERT(ciao_da_pop(&xs, NULL));
  TEST_ASSERT(ciao_da_pop(&xs, NULL));

  TEST_CHECK(ciao_da_insert(&xs, 1, 3) == false) {
    log_error("Should not be allowed to insert at index greater than da len");
    log_info("DArray elements were just popped and this index should be invalid");
    log_info("DArray len: %zu", xs.len);
  }

  
  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

TEST_FN(swap_elements) {
  CIAO_DynamicArray(int) xs = {0};

  TEST_ASSERT(ciao_da_push(&xs, 20));
  TEST_ASSERT(ciao_da_push(&xs, 50));
  TEST_ASSERT(ciao_da_push(&xs, 70));
  TEST_ASSERT(ciao_da_push(&xs, 100));

  TEST_CHECK(ciao_da_swap(&xs, 0, xs.len-1) && ciao_da_get(&xs, 0) == 100 && ciao_da_get(&xs, xs.len-1) == 20) {
    log_info("Dynamic array values:");
    fprintf(stderr, "[ ");
    ciao_da_foreach(int, it, &xs) {
      int x = *it;
      if (it == xs.items) fprintf(stderr, "%d", x);
      else fprintf(stderr, ", %d", x);
    }
    fprintf(stderr, " ]");
  }

  TEST_CHECK(ciao_da_swap(&xs, 1, 0) && ciao_da_get(&xs, 0) == 50 && ciao_da_get(&xs, 1) == 100) {
    log_info("Dynamic array values:");
    fprintf(stderr, "[ ");
    ciao_da_foreach(int, it, &xs) {
      int x = *it;
      if (it == xs.items) fprintf(stderr, "%d", x);
      else fprintf(stderr, ", %d", x);
    }
    fprintf(stderr, " ]");
  }

  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

TEST_FN(push_many_overlap) {
  CIAO_DynamicArray(int) xs = {0};

  TEST_ASSERT(ciao_da_push(&xs, 2));
  TEST_ASSERT(ciao_da_push(&xs, 3));
  TEST_ASSERT(ciao_da_push(&xs, 6));

  TEST_ASSERT(ciao_da_push_da(&xs, &xs) != true);
  TEST_ASSERT(ciao_da_push_arr(&xs, xs.items, xs.len) != true);

  TEST_EPILOGUE();
  if (xs.items) free(xs.items);
  TEST_EXIT();
}

void register_tests(Tests_Registry *registry) {
  Test_Fn_Reg tests[] = {
    TEST_REG(basic_push_and_pop),
    TEST_REG(accessor_and_find),
    TEST_REG(reserving_memory),
    TEST_REG(inserting_at_extremes),
    TEST_REG(insert_in_middle),
    TEST_REG(insert_out_of_bounds),
    TEST_REG(swap_elements),
    TEST_REG(push_many_overlap),
  };
  nob_da_append_many(registry, tests, NOB_ARRAY_LEN(tests));
}

const char *get_tests_title() {
  return "ciao_ca.h : The crow goes both cah and caw";
}
