#include "nob.h"

const char *program_name;
#define BUILD_FOLDER "./build"
#define TESTS_FOLDER "./tests"

enum: uint8_t {
  TEST_RUN_ALL   = 0,
  TEST_RUN_CROW  = 1 << 0,
  TEST_RUN_VISTA = 1 << 1,
  TEST_RUN_STRUT = 1 << 2,
};

void print_usage() {
  printf("Usage: %s [FLAG] [TEST-NAME..]\n", program_name);
  printf("FLAGS:\n");
  printf("    -h,--help        ----    Print this help message\n");
  printf("Test names:\n");
  printf("    cah,crow         ----    Test jmnuf_ca.h, the dynamic arrays\n");
}

int main(int argc, char **argv) {
  program_name = shift(argv, argc);

  uint8_t requested = 0;
  while (argc) {
    const char *arg = shift(argv, argc);

    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      print_usage();
      return 0;
    }

    if (strcmp(arg, "cah") == 0 || strcmp(arg, "crow")) {
      requested = requested | TEST_RUN_CROW;
      continue;
    }

    nob_log(ERROR, "Unknown argument: %s", arg);
    print_usage();
    return 1;
  }

  bool run_all = requested == TEST_RUN_ALL;
  int result = 0;
  Cmd cmd = {0};

  if (run_all || requested & TEST_RUN_CROW) {
    const char *output_path = BUILD_FOLDER"/crows";
    const char *input_paths[] = {
      TESTS_FOLDER"/test_ca.c",
      TESTS_FOLDER"/test.h",
      "./jmnuf_ca.h",
    };
    if (nob_needs_rebuild(output_path, input_paths, ARRAY_LEN(input_paths))) {
      nob_cc(&cmd);
      nob_cc_flags(&cmd);
      // We have a hand rolled memcpy and memmove for limited environments,
      // this is to try to make the compiler not optimize it into actual memcpy and memmove
      cmd_append(&cmd, "-O0");
      cmd_append(&cmd, "-I.");
      cmd_append(&cmd, "-Wno-unused-label");
      nob_cc_inputs(&cmd, input_paths[0]);
      nob_cc_output(&cmd, output_path);
      if (!cmd_run(&cmd)) return_defer(1);
    }
    cmd_append(&cmd, output_path);
    if (!cmd_run(&cmd)) result = 1;
  }

  if (run_all || requested & TEST_RUN_VISTA) {
    const char *output_path = BUILD_FOLDER"/vista";
    const char *input_paths[] = {
      TESTS_FOLDER"/test_vista.c",
      TESTS_FOLDER"/test.h",
      "./jmnuf_vista.h",
    };
    if (nob_needs_rebuild(output_path, input_paths, ARRAY_LEN(input_paths))) {
      nob_cc(&cmd);
      nob_cc_flags(&cmd);
      cmd_append(&cmd, "-I.");
      cmd_append(&cmd, "-Wno-unused-label");
      nob_cc_inputs(&cmd, input_paths[0]);
      nob_cc_output(&cmd, output_path);
      if (!cmd_run(&cmd)) return_defer(1);
    }
    cmd_append(&cmd, output_path);
    if (!cmd_run(&cmd)) result = 1;
  }

defer:
  if (cmd.items) free(cmd.items);
  return result;
}

#define NOB_IMPLEMENTATION
#include "nob.h"

