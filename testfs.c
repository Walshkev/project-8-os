#define CTEST_ENABLE

#include <stdio.h>
#include "image.h"
#include "block.h"
#include "ctest.h"
#include <string.h>
#include "free.h"
#include "inode.h"
#include "inode.c"

void test_image(void)
{
    CTEST_ASSERT(image_open("file_name.txt", 1) != -1, "testing trunk is 0 ");
    CTEST_ASSERT(image_close() != -1, "testing close ");
}

void test_block(void)
{
    CTEST_ASSERT(image_open("file_name.txt", 1) != -1, "testing trunk is 0 ");
    int block_num = 1;
    unsigned char block[BLOCK_SIZE] = {0};
    unsigned char block1[BLOCK_SIZE] = "foo";

    bwrite(block_num, block);
    bread(block_num, block1);

    CTEST_ASSERT(memcmp(block, block1, BLOCK_SIZE) == 0, "testing block");

    image_close();
}

void test_set_free(void)
{
    unsigned char block[BLOCK_SIZE] = {0};
    // Make a full block
    int num = 2047;
    int byte_num = num / 8;

    // Test setting a bit to 1
    set_free(block, num, 1);

    CTEST_ASSERT(block[byte_num] == 128, "test set free 1");

    // Test setting a bit to 0
    set_free(block, num, 0);

    CTEST_ASSERT(block[byte_num] == 0, "test set free 0");
}

void test_find_free(void)
{
    unsigned char block[BLOCK_SIZE];
    // Fill up block with 0xFF
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        block[i] = 0xFF;
    }

    int freeEntry = find_free(block);
    // printf("Free entry found at index: %d\n", freeEntry);
    CTEST_ASSERT(freeEntry == -1, "test set free 0");
    // Test set_free function
    set_free(block, 10, 0);

    freeEntry = find_free(block);
    // printf("Free entry found at index: %d\n", freeEntry);
    CTEST_ASSERT(freeEntry == 10, "test set free 0");
}

void test_inode_ialloc(void)
{

    // Fill up block with 0xFF
    unsigned char inode_map[BLOCK_SIZE];
    bwrite(INODE_BLOCK_NUM, inode_map);

    struct inode *free_inode = ialloc();
    CTEST_ASSERT(free_inode == NULL, "test inode when empty  ");
    // printf("Free inode found at index: %d\n", free_inode);

    struct inode *second_free_node = ialloc();

    CTEST_ASSERT(second_free_node == NULL, "test inode when full ");

    // printf("Free inode found at index: %d\n", free_inode);

    struct inode *no_free_node = ialloc();
    CTEST_ASSERT(no_free_node != NULL, "test inode when not full ");
    CTEST_ASSERT(image_close()==0 , "closing image file ");
}

void test_alloc(void)
{

    CTEST_ASSERT(image_open("file_name.txt", 1) != -1, "testing trunk is 0 ");
    // Fill up block with 0xFF
    unsigned char block_map[BLOCK_SIZE] = {0};

    bwrite(BLOCK_NUM, block_map);
    bread(BLOCK_NUM, block_map);

    int block1 = alloc();
    CTEST_ASSERT(block1 == 0, "test inode when empty  ");
    // printf("Free inode found at index: %d\n", free_inode);

    int block2 = alloc();

    CTEST_ASSERT(block2 == 1, "test inode when full ");

    // printf("Free inode found at index: %d\n", free_inode);

    int block3 = alloc();
    CTEST_ASSERT(block3 != -1, "test inode when not full ");
}

void test_incore_find_free(void)
{
    struct inode *free_node = incore_find_free();
    CTEST_ASSERT(free_node != NULL, "test find free inode when not full ");
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {

        incore[i].ref_count = 1;
    }

    struct inode *free_node1 = incore_find_free();
    CTEST_ASSERT(free_node1 == NULL, "test find free inode when not  ");
    incore_free_all();
}

void test_incore_find(void)
{
    int num = 3;

    struct inode *bob = incore_find_free();

    bob->inode_num = num;

    struct inode *found_node = incore_find(num);
    CTEST_ASSERT(found_node == bob, "test incore find \n ");
}
void test_read_inode(void){
    
    // Assert the values read from the block are correct
    struct inode test_node;

    test_node.inode_num=39;
    test_node.size =41; 
    test_node.owner_id =42;
    test_node.permissions=43;
    test_node.flags=42;
    test_node.link_count=47;
    for( int i =0 ; i< INODE_PTR_COUNT ; i++ ){
        test_node.block_ptr[i]=i;

    }  
    write_inode(&test_node);
    struct inode real_node ;

    read_inode(&real_node, test_node.inode_num);
    
    CTEST_ASSERT(real_node.size== 41, "test read_inode owner_id");
    CTEST_ASSERT(real_node.owner_id == 42, "test read_inode owner_id");
    CTEST_ASSERT(real_node.permissions == 43, "test read_inode permissions");
    CTEST_ASSERT(real_node.flags == 42, "test read_inode flags");
    CTEST_ASSERT(real_node.link_count == 47, "test read_inode link_count");
    // Assert the values in in.block_ptr are correct
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        CTEST_ASSERT(real_node.block_ptr[i] == i, "test read_inode block_ptr");
    }
    incore_free_all();
    
}

void test_iget(void){
    
    int inode_num = 5;
    int inode_random_num =-1;
 


    struct inode *inode = iget(inode_num);


    CTEST_ASSERT(inode->ref_count == 1, "test iget where incore_find is  null ");

    struct inode *inode1  = iget(inode_num);

    CTEST_ASSERT(inode1->ref_count == 2, "test iget where incore_find already exists  ");

    fill_all_inode();
    struct inode *inode2  = iget(inode_random_num);

    CTEST_ASSERT(inode2== NULL, "test iget inode is full ");

    incore_free_all();



 
}
void test_iput(void){
    int inode_num = 5;
    struct inode *inode = iget(inode_num);
    iget(inode_num);
    CTEST_ASSERT(inode->ref_count == 2, "test iget where incore_find is  null ");
    iput(inode);



    CTEST_ASSERT(inode->ref_count == 1, "test iget where reff was 2  ");

    iput(inode);
    CTEST_ASSERT(inode->ref_count == 0, "test iget where ref was 1 ");

    iput(inode);
    CTEST_ASSERT(inode->ref_count == 0, "test iget where ref was 0 ");


    
}



int main(void)
{

    CTEST_VERBOSE(1);

    test_image();

    test_block();
    test_set_free();
    test_find_free();
    test_inode_ialloc();
    test_alloc();
    test_incore_find_free();
    test_incore_find();
    test_read_inode();
    test_iget();
    test_iput();
}
