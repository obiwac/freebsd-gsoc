#!/bin/sh

# panic: Duplicate free of 0xfffff800049ad800 from zone
# 0xfffff800041e82c0(mbuf) slab 0xfffff800049adf90(8)
#
# KDB: stack backtrace:
# db_trace_self_wrapper() at db_trace_self_wrapper+0x47/frame
# 0xfffffe0016b2c4a0
# vpanic() at vpanic+0x1e0/frame 0xfffffe0016b2c500
# panic() at panic+0x43/frame 0xfffffe0016b2c560
# uma_dbg_free() at uma_dbg_free+0x246/frame 0xfffffe0016b2c5b0
# uma_zfree_arg() at uma_zfree_arg+0x1aa/frame 0xfffffe0016b2c640
# uipc_ready() at uipc_ready+0x19f/frame 0xfffffe0016b2c690
# sendfile_iodone() at sendfile_iodone+0x342/frame 0xfffffe0016b2c6f0
# vnode_pager_generic_getpages_done_async() at
# vnode_pager_generic_getpages_done_async+0x4a/frame 0xfffffe0016b2c720
# bufdone() at bufdone+0xa1/frame 0xfffffe0016b2c7a0
# g_io_deliver() at g_io_deliver+0x35b/frame 0xfffffe0016b2c800
# g_io_deliver() at g_io_deliver+0x35b/frame 0xfffffe0016b2c860
# g_io_deliver() at g_io_deliver+0x35b/frame 0xfffffe0016b2c8c0
# g_disk_done() at g_disk_done+0x179/frame 0xfffffe0016b2c910
# dadone() at dadone+0x655/frame 0xfffffe0016b2c9a0
# xpt_done_process() at xpt_done_process+0x5b2/frame 0xfffffe0016b2ca00
# xpt_done_td() at xpt_done_td+0x175/frame 0xfffffe0016b2ca60
# fork_exit() at fork_exit+0xb0/frame 0xfffffe0016b2cab0
# fork_trampoline() at fork_trampoline+0xe/frame 0xfffffe0016b2cab0
# --- trap 0, rip = 0, rsp = 0, rbp = 0 ---

# Reproduced on r359769
# Fixed by r359779

[ `uname -p` = "i386" ] && exit 0

. ../default.cfg
cat > /tmp/syzkaller7.c <<EOF
// https://syzkaller.appspot.com/bug?id=3a27f6d4656356facc22a9329be797ef1143d0ad
// autogenerated by syzkaller (https://github.com/google/syzkaller)

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
  int iter __unused;
  for (iter = 0;; iter++) {
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
      if (current_time_ms() - start < 5 * 1000)
        continue;
      kill_and_wait(pid, &status);
      break;
    }
  }
}

uint64_t r[5] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                 0xffffffffffffffff, 0xffffffffffffffff};

void execute_one(void)
{
  long res = 0;
  memcpy((void*)0x20001180, "./file0\000", 8);
  res = syscall(SYS_open, 0x20001180, 0x8240, 0);
  if (res != -1)
    r[0] = res;
  res = syscall(SYS_socketpair, 1, 1, 0, 0x20000100);
  if (res != -1) {
    r[1] = *(uint32_t*)0x20000100;
    r[2] = *(uint32_t*)0x20000104;
  }
  memcpy((void*)0x20000480, "./file0\000", 8);
  res = syscall(SYS_open, 0x20000480, 0x80000000000206, 0);
  if (res != -1)
    r[3] = res;
  res = syscall(SYS_dup, r[3]);
  if (res != -1)
    r[4] = res;
  *(uint64_t*)0x20000100 = 0x200002c0;
  memcpy((void*)0x200002c0, "\xdd", 1);
  *(uint64_t*)0x20000108 = 1;
  syscall(SYS_pwritev, r[4], 0x20000100, 1, 0);
  *(uint64_t*)0x20002e80 = 0x20000540;
  memcpy((void*)0x20000540, "\x7f", 1);
  *(uint64_t*)0x20002e88 = 1;
  syscall(SYS_pwritev, r[3], 0x20002e80, 1, 0xbf24);
  memcpy((void*)0x200004c0,
         "\x89\x88\xaa\x4a\xc3\x95\x23\x77\x54\xee\x66\xf3\x8d\xa4\xae\xf3\x47"
         "\x6d\x78\xb7\x1f\xe6\x0d\xb7\x4a\x9f\xb9\xc9\x99\x91\x6c\x98",
         32);
  syscall(SYS_setsockopt, r[2], 0, 2, 0x200004c0, 0x20);
  syscall(SYS_fcntl, r[4], 4, 0x10044);
  syscall(SYS_read, r[4], 0x20000000, 0x6d999);
  syscall(SYS_sendfile, r[0], r[1], 0, 2);
}
int main(void)
{
  syscall(SYS_mmap, 0x20000000, 0x1000000, 3, 0x1012, -1, 0ul);
  loop();
  return 0;
}
EOF
mycc -o /tmp/syzkaller7 -Wall -Wextra -O2 /tmp/syzkaller7.c -lpthread ||
    exit 1

(cd /tmp; timeout 5m ./syzkaller7) &
sleep 60
while pkill -9 syzkaller7; do :; done
wait

rm -f /tmp/syzkaller7 /tmp/syzkaller7.c /tmp/syzkaller7.core /tmp/file0
exit 0
