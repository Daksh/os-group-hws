#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/mman.h>

#define NUM_PAGES 1024
#define PAGE_SIZE 4096

void write_file(int fd, char * buf) {
    int i, ret;

    for (i = 0; i < NUM_PAGES; i++) {
        ret = write(fd, buf, PAGE_SIZE);
        if (ret != PAGE_SIZE) {
            printf("unable to write\n");
            exit(0);
        }
    }
}

void read_file(void * mappedMemory, char * buf) {
    int i, ret;
    char str[PAGE_SIZE];

    for (i = 0; i < NUM_PAGES; i++) {
        if (memcmp(mappedMemory, buf, PAGE_SIZE) != 0) {
            printf("data don’t match\n");
            exit(0);
        }
    }
}

int main() {
    int i, fd, ret;
    char buf[PAGE_SIZE];

    for (i = 0; i < PAGE_SIZE; i++)
        buf[i] = rand() % 128;
    fd = creat("/tmp/temp.txt", S_IRUSR | S_IWUSR);
    if (fd < 0) {
        printf("error in creat\n");
        exit(0);
    }
    write_file(fd, buf);
    close(fd);
    
    fd = open("/tmp/temp.txt", O_RDONLY);
    
    //Mapping the File to Memory
    //     mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
    void * mappedMemory = mmap(NULL, NUM_PAGES*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if(mappedMemory == (void *) -1)
        exit(1);

    for (i = 0; i < 1024; i++)
        read_file(mappedMemory, buf);

    //unmap: Releasing the memory
    munmap (mappedMemory, NUM_PAGES*PAGE_SIZE);

    close(fd);
    return 0;
}