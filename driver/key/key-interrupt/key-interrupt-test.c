#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define KEY_DEVICE "/dev/key-interrupt"


void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: get key using interrupt\n");
    printf("Usage: %s\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    int fd;
    unsigned char key_val;

    // open device
    fd = open(KEY_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("can't open device: %s\n", KEY_DEVICE);
        return -1;
    }
    printf("open device: %s success\n", KEY_DEVICE);

    while (1) {
        read(fd, &key_val, sizeof(key_val));
        printf("key_val = 0x%02x\n", key_val);
    }

    return 0;
}


