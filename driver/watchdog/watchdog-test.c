#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/watchdog.h>


#define WATCHDOG_DEVICE "/dev/watchdog"


void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: watchdog control\n");
    printf("Usage: %s <timeout>\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    int fd;
    int timeout;

    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    timeout = atoi(argv[1]);

    // open device
    fd = open(WATCHDOG_DEVICE, O_RDWR | O_NDELAY);
    if (fd == -1) {
		//perror("watchdog");
        printf("can't open device %s\n", WATCHDOG_DEVICE);
        return -1;
    }
    printf("open device %s success\n", WATCHDOG_DEVICE);


    printf("set watchdog timeout: %d (s)\n", timeout);
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);

    printf("sleep %d (s)\n", timeout-1);
    sleep(timeout-1);


    printf("keepalive once\n");
    ioctl(fd, WDIOC_KEEPALIVE, 0);
    printf("sleep %d (s)\n", timeout+1);
    sleep(timeout+1);

    return 0;
}


