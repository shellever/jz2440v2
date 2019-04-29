#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>


#define KEY_DEVICE "/dev/key-poll"


void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: get key using polling\n");
    printf("Usage: %s\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    int fd;
    unsigned char key_val;
    struct pollfd fds[1];
    int ret;

    // open device
    fd = open(KEY_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("can't open device: %s\n", KEY_DEVICE);
        return -1;
    }

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    while (1) {
        ret = poll(fds, 1, -1);   // -1 - wait forever, timeout 5000ms
        if (ret == 0) {
            printf("polling timeout\n");
            continue;
        }

        read(fd, &key_val, sizeof(key_val));
        printf("key_val = 0x%02x\n", key_val);  // 0x01 0x81
    }

    return 0;
}


