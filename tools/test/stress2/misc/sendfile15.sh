#!/bin/sh

#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2018 Dell EMC Isilon
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

# sendfile() over socket pairs test.

# Page fault in unp_dispose+0x90 seen:
# https://people.freebsd.org/~pho/stress/log/sendfile15.txt

# "panic: bad pte va 800657000 pte 0" seen:
# https://people.freebsd.org/~pho/stress/log/sendfile15-2.txt

# "panic: vm_page_free_prep 0xfffff817e0efac10 PG_ZERO 87 ...":
# https://people.freebsd.org/~pho/stress/log/sendfile15-2.txt
# Note that r334783 seems to alleviate the problem.

. ../default.cfg
[ `id -u` -ne 0 ] && echo "Must be root!" && exit 1

dir=/tmp
odir=`pwd`
cd $dir
sed '1,/^EOF/d' < $odir/$0 > $dir/sendfile15.c
mycc -o sendfile15 -Wall -Wextra -O0 -g sendfile15.c || exit 1
rm -f sendfile15.c
cd $odir

set -e
mount | grep "on $mntpoint " | grep -q /dev/md && umount -f $mntpoint
[ -c /dev/md$mdstart ] &&  mdconfig -d -u $mdstart
mdconfig -a -t swap -s 2g -u $mdstart
newfs $newfs_flags md$mdstart > /dev/null
mount /dev/md$mdstart $mntpoint
set +e

cd $mntpoint
dd if=/dev/random of=input bs=4k count=1 status=none
(cd $odir/../testcases/swap; ./swap -t 5m -i 20 > /dev/null) &
$dir/sendfile15
s=$?
while pgrep -q swap; do
	pkill swap
done
wait
[ -f sendfile15.core -a $s -eq 0 ] &&
    { ls -l sendfile15.core; mv sendfile15.core $dir; s=1; }
cd $odir

for i in `jot 6`; do
	mount | grep -q "on $mntpoint " || break
	umount $mntpoint && break || sleep 10
	[ $i -eq 6 ] &&
	    { echo FATAL; fstat -mf $mntpoint; exit 1; }
done
mdconfig -d -u $mdstart
rm -rf $dir/sendfile15
exit $s

EOF
#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <machine/atomic.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static volatile u_int *share;

#define PARALLEL 128
#define RUNTIME (5 * 60)
#define SYNC 0

static void
test(void)
{
	struct stat st;
	off_t written, pos;
	int child, error, from, n, status, sv[2];
	char *buf;
	const char *from_name;

	atomic_add_int(&share[SYNC], 1);
	while (share[SYNC] != PARALLEL)
		;

	from_name = "input";

	if ((from = open(from_name, O_RDONLY)) == -1)
		err(1, "open read %s", from_name);

	if ((error = fstat(from, &st)) == -1)
		err(1, "stat %s", from_name);

	if ((error = socketpair(AF_UNIX, SOCK_STREAM, 0, sv)) == -1)
		err(1, "socketpair");

	child = fork();
	if (child == -1)
		err(1, "fork");
	else if (child != 0) {
		setproctitle("parent");
		close(sv[1]);
		pos = 0;
		for (;;) {
			error = sendfile(from, sv[0], pos, st.st_size - pos,
			    NULL, &written, 0);
			if (error == -1) {
				if (errno != EAGAIN)
					err(1, "sendfile");
			}
			pos += written;
			if (pos == st.st_size)
				break;
		}
		close(sv[0]);
		if (waitpid(child, &status, 0) != child)
			err(1, "waitpid(%d)", child);
	} else {
		setproctitle("child");
		close(sv[0]);
		buf = malloc(st.st_size);
		if (buf == NULL)
			err(1, "malloc %jd", st.st_size);
		pos = 0;
		for (;;) {
			written = st.st_size - pos;
			n = read(sv[1], buf + pos, written);
			if (n == -1)
				err(1, "read");
			else if (n == 0)
				errx(1, "Short read");
			pos += n;
			if (pos == st.st_size)
				break;
		}
		close(sv[1]);
		_exit(0);
	}

	_exit(0);
}

int
main(void)
{
	pid_t pids[PARALLEL];
	size_t len;
	time_t start;
	int e, i, status;

	e = 0;
	len = PAGE_SIZE;
	if ((share = mmap(NULL, len, PROT_READ | PROT_WRITE,
	    MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
		err(1, "mmap");

	start = time(NULL);
	while ((time(NULL) - start) < RUNTIME && e == 0) {
		share[SYNC] = 0;
		for (i = 0; i < PARALLEL; i++) {
			if ((pids[i] = fork()) == 0)
				test();
			if (pids[i] == -1)
				err(1, "fork()");
		}
		for (i = 0; i < PARALLEL; i++) {
			if (waitpid(pids[i], &status, 0) == -1)
				err(1, "waitpid(%d)", pids[i]);
			if (status != 0) {
				if (WIFSIGNALED(status))
					fprintf(stderr,
					    "pid %d exit signal %d\n",
					    pids[i], WTERMSIG(status));
			}
			e += status == 0 ? 0 : 1;
		}
	}

	return (e);
}
