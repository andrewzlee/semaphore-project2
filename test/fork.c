#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "shared.h"

/* test whether the second child has to wait for the first to call
   up() before it is allowed to go. Should take about 5s to run. */
bool forked() {
  semaphore s = create_semaphore(BINARY, 1);

  pid_t first = fork();
  if(first == 0) {
    /* child 1 */
    down(s);
    sleep(2);
    up(s);
    exit(0);
  } else {
    pid_t second = fork();
    if(second == 0) {
      /* child 2 */
      down(s);
      sleep(2);
      up(s);
      exit(0);
    } else {
      time_t a = time(NULL);
      while (true) {
        int status;
        pid_t done = wait(&status);
        if (done == -1) {
          if (errno == ECHILD) break; // no more child processes
        } else {
          if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            exit(1);
          }
        }
      }
      time_t b = time(NULL);
      return b - a >= 4;        /* should be at least the sum of the
                                   sleeps */
    }
  }
}

/* test whether the semaphore table is cleaned up if delete is never
   called. Creates a child process, calls create_semaphore until it
   fails, and then tries to create a semaphore in the parent
   process. */
bool cleanup() {
  pid_t first = fork();
  if(first == 0) {
    /* child process */
    while(1) {
      semaphore s = create_semaphore(BINARY, 1);
      if(s == NULL) {
        exit(0);
      }
    }
  }

  int status;
  wait(&status);

  semaphore s = create_semaphore(BINARY, 1);
  return s != NULL;
}

int main() {
  TEST_SETUP;

  TEST_RUN( forked );
  TEST_RUN( cleanup );

  TEST_FINISH;
}
