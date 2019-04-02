#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


#define KEY_DEVICE "/dev/key-simple"


/*
 * testing:
 * # ./key-simple-test
 */
void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: print key value\n");
    printf("Usage: %s\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    int fd;
    unsigned char key_vals[4];
    int cnt = 0;

    // open device
    fd = open(KEY_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("can't open device: %s\n", KEY_DEVICE);
        return -1;
    }

    while (1) {
        read(fd, key_vals, sizeof(key_vals));
        if (!key_vals[0] || !key_vals[1] || !key_vals[2] || !key_vals[3]) {
            printf("%04d key pressed: %d %d %d %d\n", cnt++, 
                    key_vals[0], key_vals[1], key_vals[2], key_vals[3]);
        }
    }

    return 0;
}


