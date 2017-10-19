
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "sctp_helper.h"

const uint16_t kServerPort = 4321;

int main(int argc, char** argv) {
  int sock_fd;
  int msg_flags;
  const ssize_t kBufSize = 512;
  char buf[kBufSize];
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  struct sctp_sndrcvinfo sri;
  struct sctp_event_subscribe events;
  int stream_increment = 1;
  socklen_t client_addr_len;
  size_t n_read;
  int status;

  if (argc == 2) {
    stream_increment = atoi(argv[1]);
  }

  sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);

  status = bind(sock_fd, (const struct sockaddr*) &server_addr,
                sizeof(server_addr));
  if (status == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  memset(&events, 0, sizeof(events));
  events.sctp_data_io_event = 1;

  status = setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
                      &events, sizeof(events));
  if (status == -1) {
    perror("setsockopt()");
    exit(EXIT_FAILURE);
  }

  status = listen(sock_fd, SOMAXCONN);
  if (status == -1) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  for ( ; ; ) {
    client_addr_len = sizeof(struct sockaddr_in);
    n_read = sctp_recvmsg(sock_fd, buf, kBufSize,
        (struct sockaddr*) &client_addr, &client_addr_len, &sri, &msg_flags);

    if (stream_increment) {
      sri.sinfo_stream++;
      if (sri.sinfo_stream >= 
          sctp_get_no_strms(sock_fd, (struct sockaddr*) &client_addr,
                            client_addr_len)) {
        sri.sinfo_stream = 0;
      }
    }

    sctp_sendmsg(sock_fd, buf, n_read, (struct sockaddr*) &client_addr,
                 client_addr_len, sri.sinfo_ppid, sri.sinfo_flags,
                 sri.sinfo_stream, 0, 0);
  }

  exit(EXIT_SUCCESS);
}
