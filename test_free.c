#include <stdio.h>
#include "free.h"

int main() {
    unsigned char block[BLOCK_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Test set_free function
    set_free(block, 10, 1);
    set_free(block, 15, 0);

    // Test find_free function
    int freeEntry = find_free(block);
    printf("Free entry found at index: %d\n", freeEntry);

    return 0;
}