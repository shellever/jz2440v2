#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define KEY_DEVICE "/dev/key-timer"


void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: get key using timer\n");
    printf("Usage: %s\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned char key_val;
    int fd;
    int ret;

    // open device
    fd = open(KEY_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("can't open device: %s\n", KEY_DEVICE);
        return -1;
    }

    while (1) {
        ret = read(fd, &key_val, sizeof(key_val));
        printf("key_val = 0x%02x, ret = %d\n", key_val, ret);  // 0x01 0x81
        //sleep(5);
    }

    return 0;
}


