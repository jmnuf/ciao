#define NOB_UNSTRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"
#undef SV_Fmt
#undef SV_Arg
#include <stddef.h>

typedef struct Test_Case {
  const char *name;
  size_t checks_count;
  size_t checks_passed;
  bool quit_early;
  bool failed;
} Test_Case;

typedef struct Test_Fn_Reg {
  const char *name;
  bool (*fn)(Test_Case test);
} Test_Fn_Reg;

#define TEST_FN(fn_name) bool test_ ## fn_name (Test_Case test)
#define TEST_REG(fn_name) { .name = #fn_name, .fn = test_ ## fn_name }

#define log_info(...)    nob_log(NOB_INFO, __VA_ARGS__)
#define log_warning(...) nob_log(NOB_WARNING, __VA_ARGS__)
#define log_error(...)   nob_log(NOB_ERROR, __VA_ARGS__)

/*
 * @param t_name String name of test case that was running
 * @param cond String representation for the condition ran
 */
#define TEST_PRINT_FAILURE(t_name, cond) fprintf(stderr, "%s:%d: [%s] failed check: %s\n", __FILE__, __LINE__, t_name, (cond))

/*
 * @param test Pointer to a Test_Case struct
 * @param cond Condition to run for test
 */
#define TEST_CHECK(cond)                                                               \
test.checks_count++;                                                                      \
if (!test.failed && (cond)) {                                                             \
  test.checks_passed++;                                                                   \
} else if (test.failed) {} else if ((TEST_PRINT_FAILURE(test.name, #cond) || true) && false) \
{} else

#define TEST_ASSERT(cond) do { TEST_CHECK(cond) { test.quit_early = true; goto test_end; } } while (0)

#define TEST_QUIT_EARLY() do { test.quit_early = true; goto test_end; } while (0)

#define TEST_CASE_PRINT_RESULT() \
fprintf(stderr, "%s [%s]: %zu/%zu\n", (test.checks_count == test.checks_passed ? "✅" : "❌"), test.name, test.checks_count, test.checks_passed)

#define TEST_EPILOGUE()                                                                      \
test_end:                                                                                    \
if (!test.quit_early) {                                                                      \
  TEST_CASE_PRINT_RESULT();                                                                  \
} else {                                                                                     \
  fprintf(stderr, "    ❌ [%s] Got to %zu checks and quit early\n", test.name, test.checks_count); \
}


#define TEST_EXIT() return !test.failed

#define TEST_EPILOGUE_AND_EXIT() \
TEST_EPILOGUE();                 \
TEST_EXIT()

typedef struct Tests_Registry {
  Test_Fn_Reg *items;
  size_t count;
  size_t passed;
  size_t capacity;
} Tests_Registry;

void register_tests(Tests_Registry *registry);
const char *get_tests_title();

int main(void) {
  Tests_Registry registry = {0};
  register_tests(&registry);

  const char *title = get_tests_title();
  printf("==================================================\n");
  printf("|  %s\n", title);
  printf("--------------------------------------------------\n");

  registry.passed = 0;
  nob_da_foreach(Test_Fn_Reg, it, &registry) {
    if (it->fn((Test_Case) { .name = it->name })) registry.passed++;
  }

  if (registry.items) free(registry.items);

  int result;
  printf("--------------------------------------------------\n| ");
  if (registry.passed < registry.count) {
    if (registry.passed == 0) {
      printf("No tests have passed, like how did you end up here?\n");
    } else {
      printf("%zu/%zu tests passed\n", registry.passed, registry.count);
    }
    result = 1;
  } else {
    printf("All tests are passing\n");
    result = 0;
  }
  printf("--------------------------------------------------\n");
  return result;
}
