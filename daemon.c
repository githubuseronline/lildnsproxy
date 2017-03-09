
//
// Created by neitsab on 09.03.17.
//
/* 
 * Make dnsproxy a daemon and checking there is only 
 * one instanse executing at moment using pid file.
 */

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 16
#define DEV_NULL "/dev/null"
#define PID_FILE "/run/dnsproxy.pid"

static void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGHUP:
            /* can handle */
            break;
        case SIGTERM:
            unlink(PID_FILE);
            exit(0);
            break;
    }
}

static int lock_pid(const char *logfile)
{
    int fd;
    char buf[BUF_SIZE];
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if ((fd = open(logfile, O_WRONLY | O_CREAT | O_EXCL, mode)) == -1) {
        /* O_EXCL flag means the call fails if the file already exist */
        return -1;
    }

    if (ftruncate(fd, 0) == -1) {
        exit(-1);
    }

    snprintf(buf, BUF_SIZE, "%ld\n", (long) getpid());

    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
        exit(-1);
    }

    return fd;
}

int daemonize(void)
{
    int pid, fd;

    if ((pid = fork())< 0) {
        exit(-1);
    }
    else if (pid) {
        exit(0);
    }

    if (!setsid()) {
        exit(-1);
    }

    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    if ((pid = fork()) < 0) {
        exit(-1);
    }
    else if (pid) {
        exit(0);
    }

    umask(027);

    chdir("/");

    for(fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
        close(fd);
    }

    open(DEV_NULL, O_RDONLY);
    open(DEV_NULL, O_RDWR);
    open(DEV_NULL, O_RDWR);

    if (lock_pid(PID_FILE) == -1) {
        /* one dnsproxy is enough */
        exit(0);
    }

    signal(SIGTERM, signal_handler);

    return 0;
}
