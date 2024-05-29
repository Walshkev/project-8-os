#include <string.h>
#include "inode.h" 
#include "block.h" 
#include "dir.h"
#include <stdlib.h>
#include "pack.h"





void mkfs() {
   
    struct inode *inode_num = ialloc();

   
    int block_num = alloc();

 
    struct inode *root_inode = iget(inode_num->inode_num);


    root_inode->flags = 2;
    root_inode->size = 64;
    root_inode->block_ptr[0] = block_num;
    iput(root_inode);

   
    unsigned char block[BLOCK_SIZE];

    int offset = 0;


    write_u16(block + offset, inode_num->inode_num);
    offset += DIRECTORY_NAME_OFFSET;
    strcpy((char *)(block + offset), ".");
    offset += DIR_ENTRY_SIZE;
    write_u16(block + offset, inode_num->inode_num);
    offset += DIRECTORY_NAME_OFFSET;
    strcpy((char *)(block + offset), "..");
    offset += DIR_ENTRY_SIZE;

    bwrite(block_num, block);

    iput(root_inode);

}

struct directory *directory_open(int inode_num){
    struct inode *dir_inode = iget(inode_num);
    if (dir_inode == NULL) {
        return NULL; // Error: Failed to get the inode
    }

    struct directory *dir = malloc(sizeof(struct directory));
   
    dir-> offset =0 ;
    dir->inode= dir_inode;

    return dir;
    
}

int directory_get(struct directory *dir, struct directory_entry *ent){
    if (dir->offset >= dir->inode->size) {
        return -1; // Off the end of the directory
    }

    int data_block_index = dir->offset / BLOCK_SIZE;
    int data_block_num = dir->inode->block_ptr[data_block_index];

    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    int offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char*)(block + offset_in_block + 2));

    return 0;
}

void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}

