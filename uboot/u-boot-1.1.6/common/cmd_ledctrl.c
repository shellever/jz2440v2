#include <common.h>
#include <command.h>

#define	GPFCON		(*(volatile unsigned long *)0x56000050)
#define	GPFDAT		(*(volatile unsigned long *)0x56000054)

#define	GPF4_out	(1<<(4*2))
#define	GPF5_out	(1<<(5*2))
#define	GPF6_out	(1<<(6*2))

#define	GPF4_msk	(3<<(4*2))
#define	GPF5_msk	(3<<(5*2))
#define	GPF6_msk	(3<<(6*2))


// num
// 0 - D10
// 1 - D11
// 2 - D12
// 1=off, 0=on
int do_ledctrl(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i;

    printf("argc: %d\n", argc);
    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    GPFCON &= ~(GPF4_msk | GPF5_msk | GPF6_msk);
    GPFCON |= GPF4_out | GPF5_out | GPF6_out;

    if (argc == 2) {
        if (!strcmp(argv[1], "on")) {
            GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
        } else {
            GPFDAT |= (1<<4) | (1<<5) | (1<<6);
        }
    } else if (argc == 3) {
        if (!strcmp(argv[1], "0")) {
            if (!strcmp(argv[2], "on")) {
                GPFDAT &= ~(1<<4);
            } else {
                GPFDAT |= (1<<4);
            }
        } else if (!strcmp(argv[1], "1")) {
            if (!strcmp(argv[2], "on")) {
                GPFDAT &= ~(1<<5);
            } else {
                GPFDAT |= (1<<5);
            }
        } else if (!strcmp(argv[1], "2")) {
            if (!strcmp(argv[2], "on")) {
                GPFDAT &= ~(1<<6);
            } else {
                GPFDAT |= (1<<6);
            }
        }
    }

    return 0;
}

U_BOOT_CMD(
    ledctrl, 3, 1, do_ledctrl,
    "ledctrl - turn on or down the specified led\n",
    "ledctrl <num> <on|off>\n"
    "    - turn on or down the specified led\n"
    "ledctrl <on|off>\n"
    "    - turn on or down all leds\n"
);


