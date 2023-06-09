#!/bin/sh

# panic: About to free ctl:0xfffff809b0ac1260 so:0xfffff80d97dde760 and its in 1
# cpuid = 9
# time = 1605860285
# KDB: stack backtrace:
# db_trace_self_wrapper() at db_trace_self_wrapper+0x2b/frame 0xfffffe0100b1e630
# vpanic() at vpanic+0x182/frame 0xfffffe0100b1e680
# panic() at panic+0x43/frame 0xfffffe0100b1e6e0
# sctp_sorecvmsg() at sctp_sorecvmsg+0x1a96/frame 0xfffffe0100b1e810
# sctp_soreceive() at sctp_soreceive+0x1b2/frame 0xfffffe0100b1ea00
# soreceive() at soreceive+0x59/frame 0xfffffe0100b1ea20
# dofileread() at dofileread+0x81/frame 0xfffffe0100b1ea70
# sys_readv() at sys_readv+0x6e/frame 0xfffffe0100b1eac0
# amd64_syscall() at amd64_syscall+0x147/frame 0xfffffe0100b1ebf0
# fast_syscall_common() at fast_syscall_common+0xf8/frame 0xfffffe0100b1ebf0
# --- syscall (0, FreeBSD ELF64, nosys), rip = 0x8003aed4a, rsp = 0x7fffdfffdf68, rbp = 0x7fffdfffdf90 ---
# KDB: enter: panic
# [ thread pid 3933 tid 102941 ]
# Stopped at      kdb_enter+0x37: movq    $0,0x10a91b6(%rip)
# db> x/s version
# version: FreeBSD 13.0-CURRENT #0 r367842: Thu Nov 19 13:08:17 CET 2020
# pho@t2.osted.lan:/usr/src/sys/amd64/compile/PHO
# db>

[ `uname -p` != "amd64" ] && exit 0

. ../default.cfg
kldstat -v | grep -q sctp || kldload sctp.ko

cat > /tmp/syzkaller28.c <<EOF
// https://syzkaller.appspot.com/bug?id=b0e5bd1e2a4ac3caf8e2ad16ae6054d9fcc2e9d2
// autogenerated by syzkaller (https://github.com/google/syzkaller)
// Reported-by: syzbot+b2d3e3f439385340e35f@syzkaller.appspotmail.com

#define _GNU_SOURCE

#include <sys/types.h>

#include <errno.h>
#include <pthread.h>
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

static unsigned long long procid;

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

static void thread_start(void* (*fn)(void*), void* arg)
{
  pthread_t th;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 128 << 10);
  int i = 0;
  for (; i < 100; i++) {
    if (pthread_create(&th, &attr, fn, arg) == 0) {
      pthread_attr_destroy(&attr);
      return;
    }
    if (errno == EAGAIN) {
      usleep(50);
      continue;
    }
    break;
  }
  exit(1);
}

typedef struct {
  pthread_mutex_t mu;
  pthread_cond_t cv;
  int state;
} event_t;

static void event_init(event_t* ev)
{
  if (pthread_mutex_init(&ev->mu, 0))
    exit(1);
  if (pthread_cond_init(&ev->cv, 0))
    exit(1);
  ev->state = 0;
}

static void event_reset(event_t* ev)
{
  ev->state = 0;
}

static void event_set(event_t* ev)
{
  pthread_mutex_lock(&ev->mu);
  if (ev->state)
    exit(1);
  ev->state = 1;
  pthread_mutex_unlock(&ev->mu);
  pthread_cond_broadcast(&ev->cv);
}

static void event_wait(event_t* ev)
{
  pthread_mutex_lock(&ev->mu);
  while (!ev->state)
    pthread_cond_wait(&ev->cv, &ev->mu);
  pthread_mutex_unlock(&ev->mu);
}

static int event_isset(event_t* ev)
{
  pthread_mutex_lock(&ev->mu);
  int res = ev->state;
  pthread_mutex_unlock(&ev->mu);
  return res;
}

static int event_timedwait(event_t* ev, uint64_t timeout)
{
  uint64_t start = current_time_ms();
  uint64_t now = start;
  pthread_mutex_lock(&ev->mu);
  for (;;) {
    if (ev->state)
      break;
    uint64_t remain = timeout - (now - start);
    struct timespec ts;
    ts.tv_sec = remain / 1000;
    ts.tv_nsec = (remain % 1000) * 1000 * 1000;
    pthread_cond_timedwait(&ev->cv, &ev->mu, &ts);
    now = current_time_ms();
    if (now - start > timeout)
      break;
  }
  int res = ev->state;
  pthread_mutex_unlock(&ev->mu);
  return res;
}

struct thread_t {
  int created, call;
  event_t ready, done;
};

static struct thread_t threads[16];
static void execute_call(int call);
static int running;

static void* thr(void* arg)
{
  struct thread_t* th = (struct thread_t*)arg;
  for (;;) {
    event_wait(&th->ready);
    event_reset(&th->ready);
    execute_call(th->call);
    __atomic_fetch_sub(&running, 1, __ATOMIC_RELAXED);
    event_set(&th->done);
  }
  return 0;
}

static void execute_one(void)
{
  int i, call, thread;
  for (call = 0; call < 9; call++) {
    for (thread = 0; thread < (int)(sizeof(threads) / sizeof(threads[0]));
         thread++) {
      struct thread_t* th = &threads[thread];
      if (!th->created) {
        th->created = 1;
        event_init(&th->ready);
        event_init(&th->done);
        event_set(&th->done);
        thread_start(thr, th);
      }
      if (!event_isset(&th->done))
        continue;
      event_reset(&th->done);
      th->call = call;
      __atomic_fetch_add(&running, 1, __ATOMIC_RELAXED);
      event_set(&th->ready);
      event_timedwait(&th->done, 45);
      break;
    }
  }
  for (i = 0; i < 100 && __atomic_load_n(&running, __ATOMIC_RELAXED); i++)
    sleep_ms(1);
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
      if (current_time_ms() - start < 5 * 1000)
        continue;
      kill_and_wait(pid, &status);
      break;
    }
  }
}

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};

void execute_call(int call)
{
  intptr_t res = 0;
  switch (call) {
  case 0:
    res = syscall(SYS_socket, 0x1cul, 1ul, 0x84);
    if (res != -1)
      r[0] = res;
    break;
  case 1:
    *(uint32_t*)0x20000040 = 0;
    syscall(SYS_setsockopt, r[0], 0x84, 0x11, 0x20000040ul, 4ul);
    break;
  case 2:
    *(uint8_t*)0x20000000 = 0x1c;
    *(uint8_t*)0x20000001 = 0x1c;
    *(uint16_t*)0x20000002 = htobe16(0x4e22 + procid * 4);
    *(uint32_t*)0x20000004 = 0;
    *(uint8_t*)0x20000008 = 0;
    *(uint8_t*)0x20000009 = 0;
    *(uint8_t*)0x2000000a = 0;
    *(uint8_t*)0x2000000b = 0;
    *(uint8_t*)0x2000000c = 0;
    *(uint8_t*)0x2000000d = 0;
    *(uint8_t*)0x2000000e = 0;
    *(uint8_t*)0x2000000f = 0;
    *(uint8_t*)0x20000010 = 0;
    *(uint8_t*)0x20000011 = 0;
    *(uint8_t*)0x20000012 = 0;
    *(uint8_t*)0x20000013 = 0;
    *(uint8_t*)0x20000014 = 0;
    *(uint8_t*)0x20000015 = 0;
    *(uint8_t*)0x20000016 = 0;
    *(uint8_t*)0x20000017 = 0;
    *(uint32_t*)0x20000018 = 6;
    syscall(SYS_bind, r[0], 0x20000000ul, 0x1cul);
    break;
  case 3:
    *(uint8_t*)0x20000180 = 0x5f;
    *(uint8_t*)0x20000181 = 0x1c;
    *(uint16_t*)0x20000182 = htobe16(0x4e22 + procid * 4);
    *(uint32_t*)0x20000184 = 0;
    *(uint64_t*)0x20000188 = htobe64(0);
    *(uint64_t*)0x20000190 = htobe64(1);
    *(uint32_t*)0x20000198 = 0;
    syscall(SYS_connect, r[0], 0x20000180ul, 0x1cul);
    break;
  case 4:
    *(uint64_t*)0x20001500 = 0x20000200;
    *(uint64_t*)0x20001508 = 0xb8;
    *(uint64_t*)0x20001510 = 0;
    *(uint64_t*)0x20001518 = 0;
    *(uint64_t*)0x20001520 = 0;
    *(uint64_t*)0x20001528 = 0;
    *(uint64_t*)0x20001530 = 0;
    *(uint64_t*)0x20001538 = 0;
    *(uint64_t*)0x20001540 = 0;
    *(uint64_t*)0x20001548 = 0;
    syscall(SYS_readv, r[0], 0x20001500ul, 5ul);
    break;
  case 5:
    *(uint32_t*)0x20000140 = 0xb2;
    syscall(SYS_setsockopt, r[0], 0x84, 0x1b, 0x20000140ul, 4ul);
    break;
  case 6:
    res = syscall(SYS_fcntl, r[0], 0ul, r[0]);
    if (res != -1)
      r[1] = res;
    break;
  case 7:
    *(uint64_t*)0x200004c0 = 0;
    *(uint32_t*)0x200004c8 = 0;
    *(uint64_t*)0x200004d0 = 0x200003c0;
    *(uint64_t*)0x200003c0 = 0x200001c0;
    memcpy((void*)0x200001c0, "\xb0", 1);
    *(uint64_t*)0x200003c8 = 1;
    *(uint32_t*)0x200004d8 = 1;
    *(uint64_t*)0x200004e0 = 0;
    *(uint32_t*)0x200004e8 = 0;
    *(uint32_t*)0x200004ec = 0;
    syscall(SYS_sendmsg, r[1], 0x200004c0ul, 0ul);
    break;
  case 8:
    syscall(SYS_shutdown, r[0], 1ul);
    break;
  }
}
int main(void)
{
  syscall(SYS_mmap, 0x20000000ul, 0x1000000ul, 7ul, 0x1012ul, -1, 0ul);
  for (procid = 0; procid < 4; procid++) {
    if (fork() == 0) {
      loop();
    }
  }
  sleep(1000000);
  return 0;
}
EOF
mycc -o /tmp/syzkaller28 -Wall -Wextra -O0 /tmp/syzkaller28.c -lpthread ||
    exit 1

(cd ../testcases/swap; ./swap -t 1m -i 20 -h > /dev/null 2>&1) &
(cd /tmp; timeout 3m ./syzkaller28)
while pkill swap; do :; done
wait

rm -rf /tmp/syzkaller28 /tmp/syzkaller28.c /tmp/syzkaller.*
exit 0
