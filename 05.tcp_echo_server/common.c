
#include "common.h"

#include <signal.h>
#include <stdio.h>

const uint16_t kServerPort = 9001;

SigFunc BindSignal(int signo, SigFunc func, int restart) {
  struct sigaction act, old_act;
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  // Restart accept() when it is interrupted.
  if (signo != SIGALRM && restart != 0) {
    act.sa_flags |= SA_RESTART;
  }

  if (sigaction(signo, &act, &old_act) < 0) {
    perror("sigaction()");
    return SIG_ERR;
  }
  return old_act.sa_handler;
}
