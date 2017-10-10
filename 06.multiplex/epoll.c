
// Ref: http://www.cnblogs.com/haippy/archive/2012/01/09/2317269.html

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const uint16_t kServerPort = 12345;

static int set_fd_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl");
    return -1;
  }
  flags |= O_NONBLOCK;
  const int state = fcntl(fd, F_SETFL, flags);
  if (state == -1) {
    perror("fcntl(F_SETFL)");
    return -1;
  }
  return 0;
}

static int create_socket_server() {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    return -1;
  }

  // Set server socket fd non-blocking.
  int state = set_fd_nonblocking(sock_fd);
  if (state == -1) {
    return -1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);
  server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  state = bind(sock_fd, (const struct sockaddr*) &server_addr,
               sizeof(server_addr));
  if (state == -1) {
    perror("bind()");
    return -1;
  }

  return sock_fd;
}

int main(void) {
  int sock_fd = create_socket_server();
  if (sock_fd == -1) {
    exit(EXIT_FAILURE);
  }

  // Listen to incoming connections.
  int state = listen(sock_fd, SOMAXCONN);
  if (state == -1) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  // Create epoll object.
  int poll_fd = epoll_create1(0);
  if (poll_fd == -1) {
    perror("epoll_create1()");
    exit(EXIT_FAILURE);
  }

  // Add server socket fd into poll.
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.data.fd = sock_fd;
  event.events = EPOLLIN | EPOLLET;
  state = epoll_ctl(poll_fd, EPOLL_CTL_ADD, sock_fd, &event);
  if (state == -1) {
    perror("epoll_ctl(sock_fd)");
    exit(EXIT_FAILURE);
  }

  const size_t kMaxEvent = 64;
  struct epoll_event* events = calloc(kMaxEvent, sizeof(struct epoll_event));
  if (events == NULL) {
    perror("calloc()");
    exit(EXIT_FAILURE);
  }

  // Epoll event loop.
  for ( ; ; ) {
    int n_events = epoll_wait(poll_fd, events, kMaxEvent, -1);

    // Read each event.
    for (int i = 0 ; i < n_events; i++) {
      if ((events[i].events & EPOLLERR) ||
          (events[i].events & EPOLLHUP) ||
          (!events[i].events & EPOLLIN)) {
        // Error occurs, or socket is not ready for reading.
        fprintf(stderr, "epoll error\n");
        if (events[i].data.fd == sock_fd) {
          abort();
        }
        close(events[i].data.fd);

      } else if (sock_fd == events[i].data.fd) {
        // One or more incoming connections reach.
        for ( ; ; ) {
          struct sockaddr client_addr;
          socklen_t in_len;
          int conn_fd;

          in_len = sizeof(client_addr);
          conn_fd = accept(sock_fd, &client_addr, &in_len);
          if (conn_fd == -1) {
            if ((errno == EAGAIN) ||
                (errno == EWOULDBLOCK)) {
              // All incoming connections have been handled.
              break;
            } else {
              perror("accept()");
              break;
            }
          }

          char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
          state = getnameinfo(&client_addr, in_len,
                              hbuf, sizeof(hbuf),
                              sbuf, sizeof(sbuf),
                              NI_NUMERICHOST | NI_NUMERICSERV);
          if (state == 0) {
            printf("Accepted connection on %d (host=%s, port=%s)\n",
                   conn_fd, hbuf, sbuf);
          }

          // Add new client socket into poll.
          state = set_fd_nonblocking(conn_fd);
          if (state == -1) {
            abort();
          }

          memset(&event, 0, sizeof(event));
          event.data.fd = conn_fd;
          event.events = EPOLLIN | EPOLLET;
          state = epoll_ctl(poll_fd, EPOLL_CTL_ADD, conn_fd, &event);
          if (state == -1) {
            perror("epoll_ctl(ADD conn_fd");
            abort();
          }
        }

      } else {
        int done = 0;

        // New data received from connected socket.
        for ( ; ; ) {
          ssize_t n_read;
          const ssize_t kBufSize = 512;
          char buf[kBufSize];
          n_read = read(events[i].data.fd, buf, kBufSize);
          if (n_read == -1) {
            // If errno is EAGAIN, all data has been read.
            if (errno != EAGAIN) {
              perror("read()");
              done = 1;
            }
            break;
          } else if (n_read == 0) {
            // EOF reached, close socket connection.
            done = 1;
            break;
          }

          state = write(STDOUT_FILENO, buf, n_read);
          if (state == -1) {
            perror("write()");
            abort();
          }
        }

        if (done == 1) {
          printf("Closing connection on %d\n", events[i].data.fd);
          state = epoll_ctl(poll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
          if (state == -1) {
            perror("epoll_ctl(DEL client sock)");
          }
          close(events[i].data.fd);
        }
      }
    }
  }

  free(events);
  close(sock_fd);
  close(poll_fd);

  exit(EXIT_SUCCESS);
}
