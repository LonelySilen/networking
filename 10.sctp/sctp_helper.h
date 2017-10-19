
#ifndef SCTP_HELPER_H_
#define SCTP_HELPER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>

sctp_assoc_t sctp_address_to_associd(int sock_fd, struct sockaddr* sa,
                                     socklen_t salen);

int sctp_get_no_strms(int sock_fd, struct sockaddr* to, socklen_t tolen);

#endif  // SCTP_HELPER_H_
