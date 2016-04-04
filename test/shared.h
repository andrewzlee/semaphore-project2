#include <stdio.h>
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TEST_RUN(T) if( T ()) {\
    passed += 1; \
    printf(ANSI_COLOR_GREEN #T ANSI_COLOR_RESET "\n");   \
  } else { \
    failed += 1; \
    printf(ANSI_COLOR_RED #T ANSI_COLOR_RESET "\n");    \
  } \
  num_tests += 1;

#define TEST_SETUP int passed = 0, failed = 0, num_tests = 0;

#define TEST_FINISH printf(ANSI_COLOR_GREEN "Passed: %d / %d" ANSI_COLOR_RESET "\n", passed, num_tests); \
  if(failed > 0) { printf(ANSI_COLOR_RED "Failed: %d" ANSI_COLOR_RESET "\n", failed); }
