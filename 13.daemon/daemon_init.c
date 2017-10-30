
#include "daemon_init.h"

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/type.h>
#include <unistd.h>

int daemon_init(const char* pname, int facility) {
  pid_t pid;
  pid = fork();
  if (pid < 0) {
    // error.
    return -1;
  } else if (pid == 0) {
    // parent terminates.
    _exit(EXIT_SUCCESS);
  }

  // Becomes session leader.
  if (setsid() < 0) {
    return -1;
  }

  signal(SIGHUP, SIG_IGN);

  pid = fork();
  if (pid < 0) {
    return -1;
  } else if (pid == 0) {
    // child 1 termintes.
    _exit(EXIT_SUCCESS);
  }

  // Child 2 continues.

  int daemon_proc =1;

  // Change working directory.
  chdir("/");

  // Close off file descriptors.
  const int kMaxFd = 64;
  int fd_iter;
  for (fd_iter = 0; fd_iter < kMaxFd; fd_iter++) {
    close(fd_iter);
  }

  // Redirect stdin, stdout and stderr.
  const char kNullDev[] = "/dev/null";
  open(kNullDev, O_RDONLY);
  open(kNullDev, O_RDWR);
  open(kNullDev, O_RDWR);

  openlog(pname, LOG_PID, facility);

  return 0;
}
