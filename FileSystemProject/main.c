//
//  main.c
//  FileSystemProject
//
//  Created by Thomas Hill on 11/2/18.
//  Copyright © 2018 Thomas Hill. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "softwaredisk.h"
#include "filesystem.h"

#define BITS_PER_BYTE  8
#define MAX_FILES 16
#define NUM_DIRECT_BLOCKS 11


struct iNode
{
    unsigned long long size;
    unsigned short direct_blocks[NUM_DIRECT_BLOCKS];
    unsigned short indirect_block;
};
struct FileInternals
{
    char name[256];
    struct iNode file_data;
    FileMode mode;
    unsigned long long position;
    struct FileInternals* next;
};


struct FileInternals * first_open_file;
const unsigned int bit_vector_length = SOFTWARE_DISK_BLOCK_SIZE/sizeof(unsigned long long);
const unsigned int num_files = SOFTWARE_DISK_BLOCK_SIZE/sizeof(struct iNode);
const unsigned int num_blocks_in_indirect_block = SOFTWARE_DISK_BLOCK_SIZE/sizeof(unsigned short);
const unsigned long long max_file_size = SOFTWARE_DISK_BLOCK_SIZE * (NUM_DIRECT_BLOCKS + num_blocks_in_indirect_block);
FSError fserror;


void fs_print_error()
{
    switch(fserror)
    {
        case FS_NONE:
            printf("FS: no error\n");
            break;
        case FS_FILE_NOT_FOUND:
            printf("FS: File Not Found\n");
            break;
        case FS_FILE_READ_ONLY:
            printf("FS: File Read Only \n");
            break;
        case FS_FILE_ALREADY_EXISTS:
            printf("FS: Already Exists \n");
            break;
        case FS_OUT_OF_SPACE:
            printf("FS: Out Of Space \n");
            break;
        case FS_FILE_OPEN:
            printf("FS: File Open\n");
            break;
        case FS_EXCEEDS_MAX_FILE_SIZE:
            printf("FS: File Exceeds Max File Size \n");
            break;
        case FS_ILLEGAL_FILENAME:
            printf("FS: Illegal Filename \n");
            break;
        case FS_FILE_NOT_OPEN:
            printf("FS: File Not Open \n");
            break;
        case FS_IO_ERROR:
            printf("FS: IO Error");
            break;
        default:
        printf("FS: Unknown error code %d.\n", fserror);
    }
    
}

/**
 * Searches bit vector for empty block if found sets it to 1 and returns the block number
 * @return block number > 0  or 0 if no empty space or on initialization of bit vector
 */
int find_and_set_empty_block()
{
    unsigned long long bit_vector[bit_vector_length];
    read_sd_block(bit_vector, 0);
    unsigned long long full = 0xFFFFFFFFFFFFFFFF;
    int position = 0;
    for(int i = 0; i < bit_vector_length; i++)
    {
        if(!(bit_vector[i]^full))
        {
            position += sizeof(unsigned long long) * BITS_PER_BYTE;
        }
        else
        {
            unsigned long long search_bit  = 1;
            for(int j = 0; j < sizeof(unsigned long long) * BITS_PER_BYTE; j++, position++)
            {
                if(! (bit_vector[i] & search_bit))
                {
                    bit_vector[i] = bit_vector[i] | search_bit;
                    write_sd_block(bit_vector, 0);
                    return position;
                }
                else
                {
                    search_bit = search_bit<<1;
                }
            }
        }
    }
    return 0;
}
/**
 * clears block returns  block number if the block is set -1 if the block isn't set
 * @param block_num - block number to clear
 */
int clear_block(unsigned int block_num)
{
    unsigned long long bit_vector[bit_vector_length];
    read_sd_block(bit_vector, 0);
    unsigned int position_in_arr = block_num / (sizeof(unsigned long long)*BITS_PER_BYTE);
    unsigned int bit_position = block_num % (sizeof(unsigned long long)*BITS_PER_BYTE);
    unsigned long long bit = 1;
    bit = bit<< bit_position;
    //bit was not set
    if(!(bit_vector[position_in_arr] & bit)) return -1;
    //clear block
    bit_vector[position_in_arr] = bit_vector[position_in_arr] & (~bit);
    write_sd_block(bit_vector, 0);
    return block_num;
}
/**
 *checks if the file system has been initialized
 *returns first software disk block
 *if 0 not initialized if not zero then initialized
*/
int is_initialized()
{
    unsigned long long bit_vector[bit_vector_length];
    read_sd_block(bit_vector, 0);
    return bit_vector[0] != 0;
}
/**
* finds an open block and writes to it
* @param data - data to write must be 512 bytes
* @return block number returned if open block is found otherwise returns 0
*/
int find_and_write_block(void * data)
{
    int block_num = find_and_set_empty_block();
    // 0 is the bit vector block so it should always be greater than 0
    if(block_num > 0)
    {
        write_sd_block(data, block_num);
    }
    return block_num;
}

/**
* initializes file system by setting the first and second blocks
*/
void initialize_fs(){
    //todo: read from sd
    first_open_file = NULL;
   /* unsigned long long bit_vector[bit_vector_length];
    bzero(bit_vector, sizeof(bit_vector[0])* bit_vector_length);
    find_and_set_empty_block();
    read_sd_block(bit_vector, 0);
    struct  iNode inode;
    inode.size =0;
    memset(inode.direct_blocks, 0,sizeof(inode.direct_blocks[0])*NUM_DIRECT_BLOCKS);
    struct iNode iNode_block[num_files];
    for(int i = 0; i <num_files; i++)
    {
        memcpy(& iNode_block[i], &inode, sizeof(struct iNode));
    }
    */
    clear_block(0);
    clear_block(1);
    find_and_set_empty_block();
    find_and_set_empty_block();
    /*read_sd_block(bit_vector, 0);
    read_sd_block(iNode_block, 1);*/
}

unsigned long file_length(File file)
{
    if(file)
    {
        return file->file_data.size;
    }
    fserror = FS_FILE_NOT_FOUND;
    return 0;
}

int file_exists(char * name)
{
    fserror = FS_NONE;
    struct iNode iNode_block[num_files];
    read_sd_block(iNode_block, 1);
    for(int i = 0; i < num_files; i++)
    {
        if(iNode_block[i].direct_blocks[0])
        {
            char first_block[512];
            read_sd_block(first_block, iNode_block[i].direct_blocks[0]);
            if(!strncmp(name,first_block , 256))
            return 1;
        }
    }
    return 0;
}

int isFileOpen(char *name)
{
    if(first_open_file)
    {
        File finder = first_open_file;
        while(finder)
        {
            if(!strncmp(finder->name, name, 256))
            return 1;
            finder = finder->next;
        }
        
    }
    return 0;
    
}

/*File open_file(char *name, FileMode mode)
{
    if(!file_exists(name))
    {
        return NULL;
    }
    if(first_open_file)
    {
        File finder = first_open_file;
        while(finder)
        {
            if(strncmp(finder->name, name, 256));
        }
    }
}*/

File create_file(char *name, FileMode mode)
{
    fserror = FS_NONE;
    if(file_exists(name))
    {
        fserror = FS_FILE_ALREADY_EXISTS;
        return NULL;
    }
    if(name[0] == '\0')
    {
        fserror = FS_ILLEGAL_FILENAME;
        return NULL;
    }
    struct iNode iNode_block [num_files];
    read_sd_block(iNode_block, 1);
    int i = 0;
    for(; i< num_files; i++)
    {
        if((iNode_block[i]).direct_blocks[0] == 0)
        break;
    }
    if(i == num_files)
    {
        return NULL;
    }
    char first[512];
    strcpy(first, name);
    int position_of_first_block = find_and_write_block(first);
    if(!position_of_first_block)
    {
            return NULL;
    }
    iNode_block[i].direct_blocks[0] = position_of_first_block;
    write_sd_block(iNode_block, 1);
    File file = malloc(sizeof( struct FileInternals));
    strcpy(file->name, name);
    file->position = 0;
    file->next = NULL;
    file->file_data = iNode_block[i];
    file->mode = mode;
    File finder = first_open_file;
    if(!finder)
    {
        first_open_file = file;
    }
    else
    {
        while(finder->next)
        {
            finder =  finder->next;
        }
        finder->next = file;
    }
    return file;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("%llu\n", max_file_size);
    init_software_disk();
    initialize_fs();
    create_file("test_file.txt", READ_ONLY);
    create_file("test_file2.txt", READ_ONLY);
    if(create_file("test_file.txt", READ_ONLY))
    {
        printf("Error should not have created\n");
    }
    else
    {
        printf("File Already exists\n");
    }
    fs_print_error();
    if(create_file("\0", READ_ONLY))
    {
        printf("Error should not have been created\n");
    }
    else
    {
        printf("File not created: Illegal File Name\n");
    }
    fs_print_error();
    printf("File: %s, %llu \n", first_open_file->name, first_open_file->position);
    printf("File 1 exists %d, file 2 exists %d should not exist %d", file_exists("test_file.txt"), file_exists("test_file2.txt"), file_exists("test"));
    
    return 0;
}
