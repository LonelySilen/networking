#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

union opt_val_t {
  int i_val;
  long l_val;
  struct linger linger_val;
  struct timeval timeval_val;
};

static char* sock_str_flag(union opt_val_t* val, int len);
static char* sock_str_int(union opt_val_t* val, int len);
static char* sock_str_linger(union opt_val_t* val, int len);
static char* sock_str_timeval(union opt_val_t* val, int len);

struct sock_opts_t {
  const char* opt_str;
  int opt_level;
  int opt_name;
  char* (*opt_val_str) (union opt_val_t* val, int len);
};

char res[128];

struct sock_opts_t g_sock_opts[] = {
  {"SO_BROADCAST", SOL_SOCKET, SO_BROADCAST, sock_str_flag},
  {"SO_DEBUG", SOL_SOCKET, SO_DEBUG, sock_str_flag},
  {"SO_DONTROUTE", SOL_SOCKET, SO_DONTROUTE, sock_str_flag},
  {"SO_ERROR", SOL_SOCKET, SO_ERROR, sock_str_int},
  {"SO_KEEPALIVE", SOL_SOCKET, SO_KEEPALIVE, sock_str_flag},
  {"SO_LINGER", SOL_SOCKET, SO_LINGER, sock_str_linger},
  {"SO_OBOBINLINE", SOL_SOCKET, SO_OOBINLINE, sock_str_flag},
  {"SO_RCVBUF", SOL_SOCKET, SO_RCVBUF, sock_str_int},
#ifdef SCTP_NODELAY
  {"SCTP_NODELAY", IPPROTO_SCTP, SCTP_NODELAY, sock_str_flag},
#endif  // SCTP_NODELAY
  {NULL, 0, 0, NULL},
};

static char* sock_str_flag(union opt_val_t* val, int len) {
  if (len != sizeof(int)) {
    snprintf(res, sizeof(res), "size (%d) not sizeof(int)", len);
  } else {
    snprintf(res, sizeof(res), "%s", (val->i_val == 0) ? "off" : "on");
  }
  return res;
}

static char* sock_str_int(union opt_val_t* val, int len) {
  if (len != sizeof(int)) {
    snprintf(res, sizeof(res), "size (%d) not sizeof(int)", len);
  } else {
    snprintf(res, sizeof(res), "%d", val->i_val);
  }
  return res;
}

static char* sock_str_linger(union opt_val_t* val, int len) {

	if (len != sizeof(struct linger)) {
		snprintf(res, sizeof(res), "size (%d) not sizeof(struct linger)", len);
  } else {
		snprintf(res, sizeof(res), "l_onoff = %s, l_linger = %d",
				     (val->linger_val.l_onoff == 0) ? "off" : "on",
             val->linger_val.l_linger);
  }
  return res;
}

int main(void) {
  int sock_fd;
  socklen_t opt_len;
  union opt_val_t opt_val;
  struct sock_opts_t* ptr;

  for (ptr = g_sock_opts; ptr->opt_str != NULL; ptr++) {
    printf("%s: ", ptr->opt_str);

    if (ptr->opt_val_str == NULL) {
      printf("(undefined)\n");
      continue;
    }

    fflush(stdout);

    switch (ptr->opt_level) {
      case SOL_SOCKET:
      case IPPROTO_IP:
      case IPPROTO_TCP: {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        break;
      }
      case IPPROTO_IPV6: {
        sock_fd = socket(AF_INET6, SOCK_STREAM, 0);
        break;
      }
      case IPPROTO_SCTP: {
        sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
        break;
      }
      default: {
        fprintf(stderr, "Can't create socket for level %d\n", ptr->opt_level);
        break;
      }
    }

    opt_len = sizeof(union opt_val_t);
    int state = getsockopt(sock_fd, ptr->opt_level, ptr->opt_name,
                           &opt_val, &opt_len);
    if (state == -1) {
      perror("getsockopt()");
    } else {
      printf("default = %s\n", ptr->opt_val_str(&opt_val, opt_len));
    }

    close(sock_fd);
  }

  exit(EXIT_SUCCESS);
}
