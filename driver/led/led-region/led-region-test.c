#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define LED_ON  0
#define LED_OFF 1


/*
 * Usage: ledtest <dev> <on|off>
 * Available device:
 * /dev/led0
 * /dev/led1
 * /dev/led2
 * /dev/leds
 */
void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: turn on/off each led through specified device name\n");
    printf("Usage: %s <dev> <on|off>\n", fname);
    printf("Example:  %s /dev/leds on\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    int fd;
    char *filename;
    char status;

    if (argc != 3) {
        print_usage(argv[0]);
        return -1;
    }

    // get file name
    filename = argv[1];

    // get status
    if (!strcmp("on", argv[2])) {
        status = LED_ON;
    } else if (!strcmp("off", argv[2])) {
        status = LED_OFF;
    } else {
        print_usage(argv[0]);
        return -1;
    }


    // open device
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        printf("error, can't open %s\n", filename);
        return -1;
    }

    // write
    write(fd, &status, 1);
    
    return 0;
}

