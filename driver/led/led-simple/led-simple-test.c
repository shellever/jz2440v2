#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define LED_DEVICE "/dev/led-simple"
#define LED_ON  0
#define LED_OFF 1


void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: turn on/off leds at the same time\n");
    printf("Usage: %s <on|off>\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
	int fd;
    int status;

    // check argument count
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    // get status
    if (!strcmp("on", argv[1])) {
        status = LED_ON;
    } else if (!strcmp("off", argv[1])) {
        status = LED_OFF;
    } else {
        print_usage(argv[0]);
        return -1;
    }

    // open device
	fd = open(LED_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("can't open device: %s\n", LED_DEVICE);
        return -1;
	}

	write(fd, &status, sizeof(status));

	return 0;
}

