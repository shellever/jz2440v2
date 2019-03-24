#include <common.h>
#include <command.h>


int do_bfconfig(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i;
    char *name;
    char bootargs[256] = {0};
    char cmd_buf[256] = {0};
    char *keyvalue; // key, key=value

    char *delim = " ";
    char *token;

    char *field;    // keyvalue or token

    unsigned char mode = -1;
    unsigned char isfirst = 1;


    //printf("argc: %d\n", argc);
    //for (i = 0; i < argc; i++) {
    //    printf("argv[%d]: %s\n", i, argv[i]);
    //}

    if (argc < 2) {
        printf("error: no invalid argument\n");
        return -1;
    }


    /*
uboot> bfconfig u root=/dev/nfs                                                
argc: 3                                                                        
argv[0]: bfconfig                                                              
argv[1]: u                                                                     
argv[2]: root=/dev/nfs                                                         
bootargs: noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0           
uboot> printenv bootargs                                                       
bootargs=noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0            
uboot>


uboot> bfconfig u root=/dev/nfs                                               
argc: 3                                                                       
argv[0]: bfconfig                                                             
argv[1]: u                                                                     
argv[2]: root=/dev/nfs                                                         
bootargs: noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0           
action: update                                                                 
keyvalue: root=/dev/nfs                                                       
uboot>


uboot> bfconfig a ip=xxx                                                                                              
cmd_buf=setenv bootargs=noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0 ip=xxx                             
uboot> bfconfig u root=/dev/nfs                                                                                       
cmd_buf=setenv bootargs=noinitrd root=/dev/nfs init=/linuxrc console=ttySAC0                                          
uboot> bfconfig d root                                                                                                
cmd_buf=setenv bootargs=noinitrd init=/linuxrc console=ttySAC0                                                        
uboot> bfconfig d noinitrd                                                                                            
cmd_buf=setenv bootargs=root=/dev/mtdblock3 init=/linuxrc console=ttySAC0                                             
uboot> bfconfig q console                                                                                             
console=ttySAC0                                                                                                       
uboot> bfconfig q                                                                                                     
noinitrd                                                                                                              
root=/dev/mtdblock3                                                                                                   
init=/linuxrc                                                                                                         
console=ttySAC0                                                                                                       
uboot> 
     */


    if (!strcmp(argv[1], "a")) {
        //printf("action: add\n");    // append or add
        if (argc != 3) {
            printf("add error: no invalid argument\n");
            return -1;
        }
        keyvalue = argv[2];
        //printf("keyvalue: %s\n", keyvalue);
        mode = 5;
    } else if (!strcmp(argv[1], "u")) {
        //printf("action: update\n");
        if (argc != 3) {
            printf("update error: no invalid argument\n");
            return -1;
        }
        keyvalue = argv[2];
        //printf("keyvalue: %s\n", keyvalue);
        mode = 1;
    } else if (!strcmp(argv[1], "d")) {
        //printf("action: delete\n");
        if (argc != 3) {
            printf("delete error: no invalid argument\n");
            return -1;
        }
        keyvalue = argv[2];
        //printf("keyvalue: %s\n", keyvalue);
        mode = 2;
    } else if (!strcmp(argv[1], "q")) {
        //printf("action: query\n");
        if (argc == 2) {
            mode = 3;
        } else if (argc == 3) {
            keyvalue = argv[2];
            //printf("keyvalue: %s\n", keyvalue);
            mode = 4;
        } else {
            printf("query error: no invalid argument\n");
            return -1;
        }
    } else {
        printf("error: no invalid action supported\n");
        return -1;
    }


    sprintf(cmd_buf, "setenv bootargs ");
    //bootargs = getenv("bootargs");
    strcpy(bootargs, getenv("bootargs"));
    //printf("bootargs: %s\n", bootargs);

    for (token = strtok(bootargs, delim); token != NULL; token = strtok(NULL, delim)) {
        field = token;

        if (mode == 1) {    // update
            if (strchr(token, '=') != NULL 
                    && !strncmp(token, keyvalue, strlen(token) - strlen(strchr(token, '=')))) {
                field = keyvalue;
            }
        } else if (mode == 2) { // delete
            if (!strncmp(token, keyvalue, strlen(keyvalue))) {
                continue;
            }
        } else if (mode == 3) { // query all
            printf("%s\n", token);
            continue;
        } else if (mode == 4) { // query
            if (!strncmp(token, keyvalue, strlen(keyvalue))) {
                printf("%s\n", token);
                continue;
            }
        }

        if (isfirst == 1) {
            isfirst = 0;
        } else {
            strcat(cmd_buf, " ");
        }
        strcat(cmd_buf, field);
    }

    if (mode == 3 || mode == 4) {
        return 0;
    }

    if (mode == 5) {
        strcat(cmd_buf, " ");
        strcat(cmd_buf, keyvalue);
    }

    printf("cmd_buf=%s\n", cmd_buf);

    run_command(cmd_buf, 0);

    return 0;
}

U_BOOT_CMD(
    bfconfig, 3, 1, do_bfconfig,
    "bfconfig- bootargs field config, such as add, update, query or delete.\n",
    "bfconfig a <field>\n"
    "    - append bootargs the specified field\n"
    "bfconfig u <field>\n"
    "    - update bootargs the specified field\n"
    "bfconfig d <field>\n"
    "    - delete bootargs the specified field\n"
    "bfconfig q [field]\n"
    "    - query bootargs the specified field or all\n"
    "Note: field will be only key, or key=value\n"
);


