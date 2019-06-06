#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>


#define CMD_MEM_CPY_NO_DMA  0
#define CMD_MEM_CPY_DMA     1

#define DMA_DEVICE "/dev/dma"


void print_usage(char *name)
{
    printf("\n");
    printf("Usage:\n");
    printf("%s <nodma | dma>\n", name);
    printf("\n");
}

int main(int argc, char **argv)
{
    int fd;
    int i = 30;

 	if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    fd = open(DMA_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("can't open %s\n", DMA_DEVICE);
        return -1;
    }

    if (strcmp(argv[1], "nodma") == 0) {
        while (i--) {
            ioctl(fd, CMD_MEM_CPY_NO_DMA);
        }
    } else if (strcmp(argv[1], "dma") == 0) {
        while (i--) {
            ioctl(fd, CMD_MEM_CPY_DMA);
        }
    } else {
        print_usage(argv[0]);
        return -1;
    }

    return 0; 	
}


