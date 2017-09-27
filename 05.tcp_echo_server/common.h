
#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

typedef void (*SigFunc)(int signo);

SigFunc BindSignal(int signo, SigFunc func, int restart);
extern const uint16_t kServerPort;

#endif  // COMMON_H_
