#!/bin/sh

[ `uname -p` != "amd64" ] && exit 0
[ `id -u ` -ne 0 ] && echo "Must be root!" && exit 1

# panic: Assertion lock == sq->sq_lock failed at ../../../kern/subr_sleepqueue.c:371
# cpuid = 1
# time = 1623487895
# KDB: stack backtrace:
# db_trace_self_wrapper() at db_trace_self_wrapper+0x2b/frame 0xfffffe06c14b4700
# vpanic() at vpanic+0x181/frame 0xfffffe06c14b4750
# panic() at panic+0x43/frame 0xfffffe06c14b47b0
# sleepq_add() at sleepq_add+0x3e6/frame 0xfffffe06c14b4800
# _sleep() at _sleep+0x20e/frame 0xfffffe06c14b48b0
# kern_sigtimedwait() at kern_sigtimedwait+0x532/frame 0xfffffe06c14b4a20
# sys_sigwaitinfo() at sys_sigwaitinfo+0x43/frame 0xfffffe06c14b4ac0
# amd64_syscall() at amd64_syscall+0x147/frame 0xfffffe06c14b4bf0
# fast_syscall_common() at fast_syscall_common+0xf8/frame 0xfffffe06c14b4bf0
# --- syscall (0, FreeBSD ELF64, nosys), rip = 0x8003af5fa, rsp = 0x7fffffffe5f8, rbp = 0x7fffffffe610 ---
# KDB: enter: panic
# [ thread pid 15370 tid 356127 ]
# Stopped at      kdb_enter+0x37: movq    $0,0x1285b4e(%rip)
# db> x/s version
# version: FreeBSD 14.0-CURRENT #0 main-n247326-2349cda44fea: Sat Jun 12 03:57:33 CEST 2021
# pho@mercat1.netperf.freebsd.org:/usr/src/sys/amd64/compile/PHO
# db> 

. ../default.cfg
cat > /tmp/syzkaller42.c <<EOF
// https://syzkaller.appspot.com/bug?id=b12f0c4dc1e73c25636e4c4d4787209d155cca0a
// autogenerated by syzkaller (https://github.com/google/syzkaller)
// Reported-by: syzbot+1d89fc2a9ef92ef64fa8@syzkaller.appspotmail.com

#define _GNU_SOURCE

#include <sys/types.h>

#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/endian.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void kill_and_wait(int pid, int* status)
{
  kill(pid, SIGKILL);
  while (waitpid(-1, status, 0) != pid) {
  }
}

static void sleep_ms(uint64_t ms)
{
  usleep(ms * 1000);
}

static uint64_t current_time_ms(void)
{
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    exit(1);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void execute_one(void);

#define WAIT_FLAGS 0

static void loop(void)
{
  int iter __unused = 0;
  for (;; iter++) {
    int pid = fork();
    if (pid < 0)
      exit(1);
    if (pid == 0) {
      execute_one();
      exit(0);
    }
    int status = 0;
    uint64_t start = current_time_ms();
    for (;;) {
      if (waitpid(-1, &status, WNOHANG | WAIT_FLAGS) == pid)
        break;
      sleep_ms(1);
      if (current_time_ms() - start < 5000) {
        continue;
      }
      kill_and_wait(pid, &status);
      break;
    }
  }
}

void execute_one(void)
{
  syscall(SYS_rfork, 0x14034ul);
  *(uint32_t*)0x20000140 = 0x80000002;
  *(uint32_t*)0x20000144 = 0xfffffff7;
  *(uint32_t*)0x20000148 = 0x41;
  *(uint32_t*)0x2000014c = 3;
  syscall(SYS_sigwaitinfo, 0x20000140ul, 0ul);
}
int main(void)
{
  syscall(SYS_mmap, 0x20000000ul, 0x1000000ul, 7ul, 0x1012ul, -1, 0ul);
  loop();
  return 0;
}
EOF
mycc -o /tmp/syzkaller42 -Wall -Wextra -O0 /tmp/syzkaller42.c -lpthread ||
    exit 1

(cd /tmp; timeout 3m ./syzkaller42)

rm -rf /tmp/syzkaller42 /tmp/syzkaller42.c /tmp/syzkaller.*
exit 0
