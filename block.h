#ifndef BLOCK_H
#define BLOCK_H
#define BLOCK_SIZE 4096
#define BITS_PER_BYTE 8 
#define BLOCK_NUM 2 

unsigned char *bread(int block_num, unsigned char *block);
void bwrite(int block_num, unsigned char *block);


int  alloc();

extern unsigned char block;

#endif