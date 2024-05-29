int image_fd;
#include <unistd.h>
#include <fcntl.h>



// int image_fd;
    // int fd = open("foo.txt", O_CREAT | O_TRUNC | O_WRONLY, 0600);
int image_open(char *filename, int truncate) {
    image_fd = open(filename, O_RDWR | O_CREAT | (truncate ? O_TRUNC : 0), 0600);
    if (image_fd == -1) {
        return -1;
    }
    return image_fd;
}


int image_close(void) {
    
    return close(image_fd);
}

