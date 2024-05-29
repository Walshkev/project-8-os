#include <stdio.h>
#include "block.h"
#include "inode.h"
#include "free.h"
#include "stdlib.h"
#include "pack.h"


static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void)
{
    // Make a block and reads it
    unsigned char inode_map[BLOCK_SIZE];
    bread(BLOCK_NUM, inode_map);
    // Finds free inode by finding a free block
    int free_inode = find_free(inode_map);
    // Checks if its not free and sets free and writes to it.
    if (free_inode != -1)
    {
        return NULL;
        // return free_inode;
    }
    struct inode *inode = iget(free_inode);

    if (inode == NULL)
    {
        return NULL;
    }

    set_free(inode_map, free_inode, 1);
    bwrite(BLOCK_NUM, inode_map);

    inode->size = 0;
    inode->owner_id = 0;
    inode->permissions = 0;
    inode->flags = 0;
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        inode->block_ptr[i] = 0;
    }
    inode->inode_num = free_inode;

    write_inode(inode);

    return inode;
}

struct inode *incore_find_free(void){
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count == 0) {
            return incore+i;
        }
    }
    return NULL;
}


void incore_free_all(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        incore[i].ref_count = 0;
    }
}

struct inode *incore_find(unsigned int inode_num) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return NULL;

}


void read_inode(struct inode *in, int inode_num)
{
    // The block we want to read from
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    // Which inode we want to read in that block, the index.
    int block_offset = inode_num % INODES_PER_BLOCK;
    // Since block is an array of bytes, we need to multiply to get the byte index
    int block_offset_bytes = block_offset * INODE_SIZE;

    // Assuming `block` is the array we read with `bread()`

    unsigned char block[BLOCK_SIZE] ;
    bread(block_num, block);

    unsigned char *inode_ptr = block + block_offset_bytes;

    in->size = read_u32(inode_ptr);
    in->owner_id = read_u16(inode_ptr + 4);
    in->permissions = read_u8(inode_ptr + 6);
    in->flags = read_u8(inode_ptr + 7);
    in->link_count = read_u8(inode_ptr + 8);
    in->block_ptr[0] = read_u16(inode_ptr + 9);
    in->block_ptr[1] = read_u16(inode_ptr + 11);
    in->block_ptr[2] = read_u16(inode_ptr + 13);
    in->block_ptr[3] = read_u16(inode_ptr + 15);
    in->block_ptr[4] = read_u16(inode_ptr + 17);
    in->block_ptr[5] = read_u16(inode_ptr + 19);
    in->block_ptr[6] = read_u16(inode_ptr + 21);
    in->block_ptr[7] = read_u16(inode_ptr + 23);
    in->block_ptr[8] = read_u16(inode_ptr + 25);
    in->block_ptr[9] = read_u16(inode_ptr + 27);
    in->block_ptr[10] = read_u16(inode_ptr + 29);
    in->block_ptr[11] = read_u16(inode_ptr + 31);
    in->block_ptr[12] = read_u16(inode_ptr + 33);
    in->block_ptr[13] = read_u16(inode_ptr + 35);
    in->block_ptr[14] = read_u16(inode_ptr + 37);
    in->block_ptr[15] = read_u16(inode_ptr + 39);
}
void write_inode(struct inode *in)
{
   
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    // inode we want to read in that block
    int block_offset = in->inode_num % INODES_PER_BLOCK;
    // Since block is an array of bytes, we need to multiply to get the byte index
    int block_offset_bytes = block_offset * INODE_SIZE;

    // Assuming `block` is the array we read with `bread()`

    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);

    unsigned char *inode_ptr = block + block_offset_bytes;

    write_u32(inode_ptr, in->size);
    write_u16(inode_ptr + 4, in->owner_id);
    write_u8(inode_ptr + 6, in->permissions);
    write_u8(inode_ptr + 7, in->flags);
    write_u8(inode_ptr + 8, in->link_count);
    write_u16(inode_ptr + 9, in->block_ptr[0]);
    write_u16(inode_ptr + 11, in->block_ptr[1]);
    write_u16(inode_ptr + 13, in->block_ptr[2]);
    write_u16(inode_ptr + 15, in->block_ptr[3]);
    write_u16(inode_ptr + 17, in->block_ptr[4]);
    write_u16(inode_ptr + 19, in->block_ptr[5]);
    write_u16(inode_ptr + 21, in->block_ptr[6]);
    write_u16(inode_ptr + 23, in->block_ptr[7]);
    write_u16(inode_ptr + 25, in->block_ptr[8]);
    write_u16(inode_ptr + 27, in->block_ptr[9]);
    write_u16(inode_ptr + 29, in->block_ptr[10]);
    write_u16(inode_ptr + 31, in->block_ptr[11]);
    write_u16(inode_ptr + 33, in->block_ptr[12]);
    write_u16(inode_ptr + 35, in->block_ptr[13]);
    write_u16(inode_ptr + 37, in->block_ptr[14]);
    write_u16(inode_ptr + 39, in->block_ptr[15]);
    bwrite(block_num , block);
}


struct inode *iget(int inode_num){
    struct inode *inode1 = incore_find(inode_num);
    
    if (inode1 != NULL) {
        inode1->ref_count++;
        return inode1;
    }
    
    inode1 = incore_find_free();
    
    if (inode1 == NULL) {
        return NULL;
    }
    
    read_inode(inode1, inode_num);
    inode1->ref_count = 1;
    inode1->inode_num = inode_num;
    
    return inode1;
}

void fill_all_inode(void){
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        
       
        incore[i].ref_count =1; 
        
    }
}
void iput(struct inode *in){
    if(in == NULL) 
        return; 
    if (in->ref_count == 0) {
        return;
    }
    in->ref_count -= 1;
    if (in->ref_count == 0) {
        write_inode(in);
    }
    
}

