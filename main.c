#include "nob.h"

const char *program_name;
#define BUILD_FOLDER "./build"
#define TESTS_FOLDER "./tests"

enum Command_Kind {
  COMMAND_NONE = 0,
  COMMAND_TEST,
};

enum Test_Run_Flag {
  TEST_RUN_ALL   = 0,
  TEST_RUN_CROW  = 1 << 0,
  TEST_RUN_VISTA = 1 << 1,
  TEST_RUN_STRUT = 1 << 2,
};

void print_usage() {
  printf("Usage: %s <commands> [FLAGS]\n", program_name);
  printf("COMMANDS:\n");
  printf("    help               ----    Print this help message\n");
  printf("    test [names..]     ----    Run tests\n");
  printf("        Test Names:\n");
  printf("            cah/caw    ----    Test ciao_ca.h, the dynamic arrays\n");
  printf("            vista      ----    Test ciao_vista.h, the string views\n");
}

int main(int argc, char **argv) {
  program_name = shift(argv, argc);

  if (argc == 0) {
    nob_log(ERROR, "No command provided.");
    print_usage();
    return 1;
  }

  uint8_t requested = 0;
  Command_Kind command = COMMAND_NONE;
  while (argc) {
    const char *arg = shift(argv, argc);

    bool ok = true;
    switch (command) {
    case COMMAND_NONE: {
      if (strcmp(arg, "help") == 0) {
        print_usage();
        return 0;
      }

      if (strcmp(arg, "test") == 0) {
        command = COMMAND_TEST;
        break;
      }

      ok = false;
      nob_log(ERROR, "Unknown command provided: %s", arg);
      print_usage();
    } break;

    case COMMAND_TEST: {
      if (strcmp(arg, "cah") == 0 || strcmp(arg, "caw")) {
        requested = requested | TEST_RUN_CROW;
      } else if (strcmp(arg, "vista") == 0) {
        requested = requested | TEST_RUN_VISTA;
      } else {
        ok = false;
        nob_log(ERROR, "Unknown tests collection specified: %s" arg);
        print_usage();
      }
    } break;
    }

    if (!ok) return 1;
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

