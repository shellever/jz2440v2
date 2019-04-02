#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#define KEY_DEVICE "/dev/key-fasync"


int fd;

void my_signal_fun(int signum)
{
    unsigned char key_val;
    read(fd, &key_val, sizeof(key_val));
    printf("key_val = 0x%02x\n", key_val);  // 0x01 0x81
}


void print_usage(char *fname)
{
    printf("\n");
    printf("Brief: get key using fasync\n");
    printf("Usage: %s\n", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
    int ret;
    int flags;

    // open device
	fd = open(KEY_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("can't open device: %s\n", KEY_DEVICE);
        return -1;
	}

    // register signal handler
    signal(SIGIO, my_signal_fun);

    // set pid (attach current process)
    fcntl(fd, F_SETOWN, getpid());

    // set FASYNC flag
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | FASYNC);


    while (1) {
        sleep(1000);
    }

	return 0;
}

