#include <common.h>
#include <command.h>


int do_printme(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i;

    printf("argc: %d\n", argc);
    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    return 0;
}

U_BOOT_CMD(
    printme, 3, 1, do_printme,
    "printme - print the information about me\n",
    "print the infor about me , such as account,\n"
    "email and address and so on.\n"
    "I will be so happy to communicate with you.\n"
);


