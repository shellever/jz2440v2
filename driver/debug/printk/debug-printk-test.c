#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define DEBUG_DEVICE    "/dev/debug-level"

#define DEBUG_IOC_CTRL  _IOW('d', 1, unsigned int)


void print_usage(const char *prog)
{
    printf("\n");
    printf("Usage: %s\n", prog);
    printf("\n");
}

int main(int argc, char *argv[])
{
    int fd;
    int val;

    fd = open(DEBUG_DEVICE, O_RDWR | O_NDELAY);
    if (fd < 0) {
        printf("can't open device: %s\n", DEBUG_DEVICE);
        return -1;
    }

    read(fd, &val, sizeof(val));

    write(fd, &val, sizeof(val));

    ioctl(fd, DEBUG_IOC_CTRL, &val);

    close(fd);

    return 0;
}

