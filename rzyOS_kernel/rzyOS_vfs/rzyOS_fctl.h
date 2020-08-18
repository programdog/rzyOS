#ifndef RZYOS_FCTL_H
#define RZYOS_FCTL_H

#include "rzyOS_fs.h"


int open(char *path, int oflag, int mode);

int close(int fd);

ssize_t read(int fd, void *buf, ssize_t count);

ssize_t write(int fd, void *buf, ssize_t count);

int ioctl(int fd, int req, unsigned long arg);


#endif
