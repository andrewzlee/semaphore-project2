#include <unistd.h>
#include <stdbool.h>

#include "shared.h"

bool create_negative() {
  semaphore s = create_semaphore(-1, -10);
  return s == NULL;
}

bool create_binary_too_big() {
  semaphore s = create_semaphore(BINARY, 10);
  return s == NULL;
}

bool create_binary() {
  semaphore s = create_semaphore(BINARY, 1);
  return s != NULL;
}

bool create_multi() {
  semaphore s = create_semaphore(MULTIVALUED, 10);
  return s != NULL;
}

bool delete_bad() {
  return delete_semaphore(10000) == 0;
}

bool delete_good() {
  semaphore s = create_semaphore(MULTIVALUED, 10);
  return delete_semaphore(s) == -1;
}

bool up_simple() {
  semaphore s = create_semaphore(MULTIVALUED, 10);
  bool res = up(s) == -1;
  delete_semaphore(s);
  return res;
}

bool down_simple() {
  semaphore s = create_semaphore(MULTIVALUED, 10);
  bool res = down(s) == -1;
  delete_semaphore(s);
  return res;
}

int main() {
  TEST_SETUP;

  TEST_RUN( create_negative );
  TEST_RUN( create_binary_too_big );
  TEST_RUN( create_binary );
  TEST_RUN( create_multi );

  TEST_RUN( delete_bad );
  TEST_RUN( delete_good );

  TEST_RUN( up_simple );
  TEST_RUN( down_simple );

  TEST_FINISH;
}
