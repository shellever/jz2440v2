#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define BUZZER_IOC_MAGIC        'b'
#define BUZZER_IOC_WR_BEEP_CTRL _IOW(BUZZER_IOC_MAGIC, 1, unsigned int)
#define BUZZER_DEVICE           "/dev/buzzer-level"

#define BUZZER_BEEP_MODE_ON     0x01
#define BUZZER_BEEP_MODE_OFF    0x02
#define BUZZER_BEEP_MODE_MS     0x03


void do_buzzer_beep(unsigned char mode, int duration)
{
    int fd_drv;
    unsigned int buf[2];

    fd_drv = open(BUZZER_DEVICE, O_RDWR | O_NDELAY);

    buf[0] = mode;
    buf[1] = duration;
    ioctl(fd_drv, BUZZER_IOC_WR_BEEP_CTRL, buf);

    close(fd_drv);
}

void buzzer_beep_on(void)
{
    do_buzzer_beep(BUZZER_BEEP_MODE_ON, 0);
}

void buzzer_beep_off(void)
{
    do_buzzer_beep(BUZZER_BEEP_MODE_OFF, 0);
}

void buzzer_beep(int duration)
{
    do_buzzer_beep(BUZZER_BEEP_MODE_MS, duration);
}


void print_usage(const char *progname)
{
    printf("\n");
    printf("Usage: %s <on|off|ms duration>\n", progname);
    printf("\n");
}

int main(int argc, char *argv[])
{
    int mode, duration;

    if (argc < 2) {
        print_usage(argv[0]);
        exit(1);
    }

    if (!strcmp(argv[1], "on")) {
        mode = BUZZER_BEEP_MODE_ON;
    } else if (!strcmp(argv[1], "off")) {
        mode = BUZZER_BEEP_MODE_OFF;
    } else if (!strcmp(argv[1], "ms")) {
        mode = BUZZER_BEEP_MODE_MS;
        duration = atoi(argv[2]);
    }

    if (mode == BUZZER_BEEP_MODE_ON) {
        buzzer_beep_on();
    } else if (mode == BUZZER_BEEP_MODE_OFF) {
        buzzer_beep_off();
    } else if (mode == BUZZER_BEEP_MODE_MS) {
        buzzer_beep(duration);
    }

    return 0;
}

