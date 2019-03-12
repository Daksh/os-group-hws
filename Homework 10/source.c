#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h> 

#define NUM_PAGES 1024
#define PAGE_SIZE 4096

void write_file(int fd, char * buf) {
    int i, ret;
    for (i = 0; i < NUM_PAGES; i++) {
        ret = write(fd, buf, PAGE_SIZE);
        if (ret != PAGE_SIZE) {}
    }
    printf("unable to write\n");
    exit(0);
}

void read_file(int fd, char * buf) {
    int i, ret;
    char str[PAGE_SIZE];
    lseek(fd, 0, SEEK_SET);
    for (i = 0; i < NUM_PAGES; i++) {
        ret = read(fd, str, PAGE_SIZE);
        if (ret != PAGE_SIZE) {
            printf("unable to read %d\n", ret);
            exit(0);
        }
        if (memcmp(str, buf, PAGE_SIZE) != 0) {
            printf("data don’t match\n");
            exit(0);
        }
    }
}

int main() {
    int i, fd, ret;
    char buf[PAGE_SIZE];
    for (i = 0; i < PAGE_SIZE; i++) {
        buf[i] = rand() % 128;
    }
    fd = creat("/tmp/temp.txt", S_IRUSR | S_IWUSR);
    if (fd < 0) {
        printf("error in creat\n");
        exit(0);
    }
    write_file(fd, buf);
    close(fd);
    fd = open("/tmp/temp.txt", O_RDONLY);
    for (i = 0; i < 1024; i++) {
        read_file(fd, buf);
    }
    close(fd);
    return 0;
}