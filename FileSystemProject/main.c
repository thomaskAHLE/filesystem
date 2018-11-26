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

//number of initail blocks = block for bitvector + block for iNodes + blocks for Directory Entries
#define NUM_INIT_BLOCKS 18
#define MAX_FILE_SIZE SOFTWARE_DISK_BLOCK_SIZE * (NUM_DIRECT_BLOCKS + num_blocks_in_indirect_block)

struct iNode
{
    unsigned long long size;
    unsigned short direct_blocks[NUM_DIRECT_BLOCKS];
    unsigned short indirect_block;
};

struct DirectoryEntry{
    char name[256];
    unsigned short iNode_number;
    char padding[SOFTWARE_DISK_BLOCK_SIZE - 256 -sizeof(unsigned short)];
};

struct FileInternals
{
    struct DirectoryEntry dir_entry;
    struct iNode iNode_data;
    FileMode mode;
    unsigned long long position;
    struct FileInternals* next;
};




struct FileInternals * first_open_file;
const unsigned int bit_vector_length = SOFTWARE_DISK_BLOCK_SIZE/sizeof(unsigned long long);
const unsigned int num_files = SOFTWARE_DISK_BLOCK_SIZE/sizeof(struct iNode);
const unsigned int num_blocks_in_indirect_block = SOFTWARE_DISK_BLOCK_SIZE/sizeof(unsigned short);

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
            printf("FS: Out Of Space or reached max file limit \n");
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
int clear_block_in_bit_vector(unsigned int block_num)
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
 * Write specified block in bit vector
 * @return block number or -1 if bit was not set
 */
int write_specific_block_in_bit_vector(unsigned int block_num)
{
    unsigned long long bit_vector[bit_vector_length];
    read_sd_block(bit_vector, 0);
    unsigned int position_in_arr = block_num / (sizeof(unsigned long long)*BITS_PER_BYTE);
    unsigned int bit_position = block_num % (sizeof(unsigned long long)*BITS_PER_BYTE);
    unsigned long long bit = 1;
    bit = bit<< bit_position;
    //bit was not set
    if((bit_vector[position_in_arr] & bit)) return -1;
    //clear block
    bit_vector[position_in_arr] = bit_vector[position_in_arr] | bit;
    return block_num;
}

/**
 * Searches bit vector for empty block and writes 'data' to it.
 * @return block number
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


unsigned long write_to_direct_blocks(File file, char * data, unsigned long numbytes, unsigned short direct_block_index, unsigned short offset_bytes )
{
    unsigned short block_number;
    unsigned long long bytes_written = 0;
    
    numbytes = strlen(data)  < numbytes ? strlen(data) : numbytes;
    unsigned long long remaining_data = numbytes;
    unsigned long long dataForBlock = 0;
    block_number = file->iNode_data.direct_blocks[direct_block_index];
    char data_to_write[SOFTWARE_DISK_BLOCK_SIZE];
    while( direct_block_index < NUM_DIRECT_BLOCKS && bytes_written < numbytes)
    {
        bzero(data_to_write, SOFTWARE_DISK_BLOCK_SIZE *sizeof(char));
        dataForBlock = (remaining_data < SOFTWARE_DISK_BLOCK_SIZE - offset_bytes? remaining_data : SOFTWARE_DISK_BLOCK_SIZE- offset_bytes);
        if(!(file->iNode_data.direct_blocks[direct_block_index]))
        {
            memcpy(data_to_write + offset_bytes, data + bytes_written, dataForBlock);
            block_number = find_and_write_block(data_to_write);
            file->iNode_data.direct_blocks[direct_block_index] = block_number;
            if(block_number)
                file->iNode_data.size += dataForBlock;
        }
        else
        {
            block_number = file->iNode_data.direct_blocks[direct_block_index];
            read_sd_block(data_to_write, block_number);
            file->iNode_data.size += dataForBlock <= strlen(data_to_write) ? 0 : dataForBlock - strlen(data_to_write);
            memcpy(data_to_write  + offset_bytes, data + bytes_written, dataForBlock);
            write_sd_block(data_to_write, block_number);
        }
        if(!block_number )
        {
            fserror = FS_OUT_OF_SPACE;
            struct iNode iNodeBlock[num_files];
            read_sd_block(iNodeBlock, 1);
            iNodeBlock[file->dir_entry.iNode_number] = file->iNode_data;
            write_sd_block(iNodeBlock, 1);
            return bytes_written;
        }
        if(dataForBlock + bytes_written == numbytes)
        {
            struct iNode iNodeBlock[num_files];
            read_sd_block(iNodeBlock, 1);
            iNodeBlock[file->dir_entry.iNode_number] = file->iNode_data;
            write_sd_block(iNodeBlock, 1);
            file->position += dataForBlock;
            return bytes_written + dataForBlock;
        }
        bytes_written += dataForBlock;
        file->position += dataForBlock;
        direct_block_index++;
        remaining_data -= dataForBlock;
        offset_bytes = 0;
    }
    struct iNode iNodeBlock[num_files];
    read_sd_block(iNodeBlock, 1);
    iNodeBlock[file->dir_entry.iNode_number] = file->iNode_data;
    write_sd_block(iNodeBlock, 1);
    return bytes_written;
}

unsigned long write_to_indirect_blocks(File file, char *data, unsigned long numbytes, unsigned short offset_bytes, unsigned short indirect_block_index)
{
    unsigned long bytes_written = 0;
    if(!(file->iNode_data.indirect_block ))
    {
        file->iNode_data.indirect_block = find_and_set_empty_block();
        struct iNode iNodeBlock[num_files];
        read_sd_block(iNodeBlock, 1);
        iNodeBlock[file->dir_entry.iNode_number] = file->iNode_data;
        write_sd_block(iNodeBlock, 1);
    }
    unsigned short indirect_block[num_blocks_in_indirect_block];
    unsigned long remaining_data = numbytes;
    read_sd_block(indirect_block,file->iNode_data.indirect_block);
    char data_to_write[SOFTWARE_DISK_BLOCK_SIZE];
    unsigned short dataForBlock = 0;
    unsigned short block_number;
    while(indirect_block_index < num_blocks_in_indirect_block && bytes_written < numbytes)
    {
        bzero(data_to_write, SOFTWARE_DISK_BLOCK_SIZE *sizeof(char));
        dataForBlock = remaining_data < SOFTWARE_DISK_BLOCK_SIZE - offset_bytes? remaining_data : SOFTWARE_DISK_BLOCK_SIZE- offset_bytes;
        if(!(indirect_block[indirect_block_index]))
        {
            memcpy(data_to_write + offset_bytes, data + bytes_written, dataForBlock);
            block_number = find_and_write_block(data_to_write);
            indirect_block[indirect_block_index] = block_number;
            if(block_number)
            file->iNode_data.size += dataForBlock;
        }
        else
        {
            block_number = indirect_block[indirect_block_index];
            read_sd_block(data_to_write, block_number);
            file->iNode_data.size += dataForBlock <= strlen(data_to_write) ? 0 : dataForBlock - strlen(data_to_write);
            memcpy(data_to_write  + offset_bytes, data + bytes_written, dataForBlock);
            write_sd_block(data_to_write, block_number);
        }
        if(!block_number )
        {
            fserror = FS_OUT_OF_SPACE;
            struct iNode iNodeBlock[num_files];
            read_sd_block(iNodeBlock, 1);
            iNodeBlock[file->dir_entry.iNode_number] = file->iNode_data;
            write_sd_block(iNodeBlock, 1);
            write_sd_block(indirect_block, file->iNode_data.indirect_block);
            file->position += dataForBlock;
            return bytes_written;
        }
        if(dataForBlock + bytes_written == numbytes)
        {
            struct iNode iNodeBlock[num_files];
            read_sd_block(iNodeBlock, 1);
            iNodeBlock[file->dir_entry.iNode_number] = file->iNode_data;
            write_sd_block(iNodeBlock, 1);
            write_sd_block(indirect_block, file->iNode_data.indirect_block);
            file->position += dataForBlock;
            return bytes_written + dataForBlock;
        }
        bytes_written += dataForBlock;
        file->position += dataForBlock;
        indirect_block_index++;
        remaining_data -= dataForBlock;
        offset_bytes = 0;
    }
    write_sd_block(indirect_block, file->iNode_data.indirect_block);
    fserror = FS_EXCEEDS_MAX_FILE_SIZE;
    return bytes_written;
}


/**
 * writes 'numbytes' of data from 'buf' into 'file' at the current file position.
 * @return number of bytes written. If out of space, return value may be lower than numbytes
 */
unsigned long write_file(File file, void *buf, unsigned long numbytes)
{
    //todo export to write to direct blocks/ write to indirect blocks
    fserror = FS_NONE;
    char * data = (char *) buf;
    if(file->mode == READ_ONLY)
    {
        fserror = FS_FILE_READ_ONLY;
        return 0;
    }
    if(file->position >= MAX_FILE_SIZE )
    {
        fserror = FS_EXCEEDS_MAX_FILE_SIZE;
        return 0;
    }
    unsigned long bytes_written = 0;
    unsigned short iNode_block_index = (file->position)/SOFTWARE_DISK_BLOCK_SIZE;
    int starting_at_direct = iNode_block_index < NUM_DIRECT_BLOCKS;
    unsigned short offset_bytes = (file->position) % SOFTWARE_DISK_BLOCK_SIZE;
    if(strlen(data) < numbytes)
    {
        numbytes = strlen(data);
    }
    if(starting_at_direct && 0 < numbytes )
    {
        bytes_written += write_to_direct_blocks(file, data,numbytes, iNode_block_index, offset_bytes);
        offset_bytes = 0;
        iNode_block_index = (file->position)/SOFTWARE_DISK_BLOCK_SIZE;
        numbytes -= bytes_written;
    }
    if(!starting_at_direct || 0 < numbytes)
    {
        bytes_written += write_to_indirect_blocks(file, data + bytes_written, numbytes, offset_bytes, iNode_block_index - NUM_DIRECT_BLOCKS);
    }
    return bytes_written;
}

//should file set a block if it is not initialized
/**
 * sets current position in file to 'bytepos', always relative to the beginning of file.
 * Seeks past the current end of file should extend the file.
 * @return 1 if success, 0 if failure
 */
int seek_file(File file, unsigned long bytepos)
{
    if(bytepos >= MAX_FILE_SIZE){
        fserror = FS_EXCEEDS_MAX_FILE_SIZE;
        return 0;
    }
    file->position = bytepos;
    fserror = FS_NONE;
    return 1;

}

/**
 * clear block at 'block_num'
 * @return block number
 */
int clear_block(unsigned int block_num)
{
    clear_block_in_bit_vector(block_num);
    char empty_block[SOFTWARE_DISK_BLOCK_SIZE];
    bzero(empty_block, SOFTWARE_DISK_BLOCK_SIZE);
    write_sd_block(empty_block, block_num);
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
    return bit_vector[0] & 1;
}


/**
 * Initializes file system
 */
void initialize_fs(){
    first_open_file = NULL;
    for(int i =0; i < NUM_INIT_BLOCKS; i++)
    clear_block_in_bit_vector(i);
    for(int i =0; i < NUM_INIT_BLOCKS; i++)
    find_and_set_empty_block();
}

/**
 * finds current length of 'file'
 * @return current length of file in bytes
 */
unsigned long file_length(File file)
{
    fserror = FS_NONE;
    if(file)
    {
        return file->iNode_data.size;
    }
    fserror = FS_FILE_NOT_FOUND;
    return 0;
}

/**
 * determines if file with 'name' exists
 * @return 1 on success or 0 on failure
 */
int file_exists(char * name)
{
    if(!is_initialized())
    {
        initialize_fs();
    }
    fserror = FS_NONE;
    struct DirectoryEntry de;
    //starting at block with first directory entry read all directory entries check for name
    for(int i = 2; i < 2 + num_files; i++)
    {
        read_sd_block(&de, i);
        if(!strncmp(de.name, name, 256))
            return 1;
    }
    return 0;
}

/**
 * checks if file with 'name' is open
 * @return 1 if open, 0 if not
 */
int is_file_open(char *name)
{
    if(!is_initialized())
    {
        initialize_fs();
    }
    if(first_open_file)
    {
        File finder = first_open_file;
        while(finder)
        {
            if(!strncmp(finder->dir_entry.name, name, 256))
                return 1;
            finder = finder->next;
        }
    }
    return 0;
    
}

/**
 * clears indirect block at 'indirect_block_number'
 */
void delete_indirect_block(int indirect_block_number)
{
    unsigned short indirect_block[num_blocks_in_indirect_block];
    for(int i = 0; i  <num_blocks_in_indirect_block; i++)
    {
        if(indirect_block[i])
        {
            clear_block(indirect_block[i]);
        }
    }
    clear_block(indirect_block_number);
}

/**
 * deletes data in iNode 'iNodeNumber'
 */
void delete_iNode_data(int iNodeNumber)
{
    struct iNode iNodeBlock[num_files];
    read_sd_block(&iNodeBlock, 1);
    for(int i = 0; i < NUM_DIRECT_BLOCKS; i++)
    {
        if(iNodeBlock[iNodeNumber].direct_blocks[i])
        {
            clear_block(iNodeBlock[iNodeNumber].direct_blocks[i]);
        }
         iNodeBlock[iNodeNumber].direct_blocks[i] = 0;
    }
    if(iNodeBlock[iNodeNumber].indirect_block)
    {
        delete_indirect_block(iNodeBlock[iNodeNumber].indirect_block);
        iNodeBlock[iNodeNumber].indirect_block = 0;
    }
    iNodeBlock[iNodeNumber].size = 0;
    write_sd_block(&iNodeNumber, 1);
}

/**
 * deletes directory entry with 'name'
 */
void delete_dir_entry(char *name)
{
    struct DirectoryEntry de;
    int de_block_number = 2;
    for(; de_block_number < num_files + 2; de_block_number++)
    {
        read_sd_block(&de, de_block_number);
        if(!strncmp(de.name, name, 256))
        {
            break;
        }
    }
    de.name[0]= '\0';
    delete_iNode_data(de.iNode_number);
    write_sd_block(&de, de_block_number);
}

//completely un-tested
/**
 * deletes the file named 'name'
 * @return 1 on success, 0 on failure
 */
int delete_file(char *name)
{
    fserror = FS_NONE;
    if(!file_exists(name))
    {
        fserror = FS_FILE_NOT_FOUND;
        return 0;
    }
    if(is_file_open(name))
    {
        fserror = FS_FILE_OPEN;
        return 0;
    }
    delete_dir_entry(name);
    return 1;
}

/**
 * adds 'file' to list of open files
 */
void add_to_open_files(File file)
{
    if(!first_open_file) first_open_file = file;
    else
    {
        File file_node_ptr = first_open_file;
        while(file_node_ptr->next)
        {
            file_node_ptr = file_node_ptr->next;
        }
        file_node_ptr->next = file;
    }
}

/**
 * close 'file'
 */
void close_file(File file)
{
    if(!is_initialized())
    {
        initialize_fs();
    }
    fserror = FS_NONE;
    if(!file)
    {
        fserror = FS_FILE_NOT_FOUND;
    }
    else if(!is_file_open(file->dir_entry.name))
    {
        fserror = FS_FILE_NOT_OPEN;
    }
    else
    {
        if(!strncmp(file->dir_entry.name, first_open_file->dir_entry.name,256))
        {
            if(!first_open_file->next)
            {
                first_open_file = NULL;
            }
            else
            {
                first_open_file = first_open_file->next;
            }
        }
        else
        {
            File file_iterator = first_open_file;
            while(strncmp(file_iterator->next->dir_entry.name, file->dir_entry.name, 256))
            {
                file_iterator = file_iterator->next;
            }
            file_iterator->next = file->next;
        }
        free(file);
    }
}

/**
 * Open existing file with 'name' and access mode 'mode'and sets current file position to byte 0
 * @return open file or null on error
 */
File open_file(char *name, FileMode mode)
{
    if(!is_initialized())
    {
        initialize_fs();
    }
    fserror = FS_NONE;
    if(!file_exists(name))
    {
        fserror = FS_FILE_NOT_FOUND;
        return NULL;
    }
    if(is_file_open(name))
    {
        File finder = first_open_file;
        while(finder)
        {
            if(!strncmp(finder->dir_entry.name, name, 256))
            break;
        }
        return finder;
    }
    File file_to_open = malloc(sizeof(struct FileInternals));
    struct DirectoryEntry de;
    for(int i = 2; i < num_files + 2; i++)
    {
        read_sd_block(&de, i);
        if(! strncmp(de.name, name, 256))
        {
            file_to_open->dir_entry = de;
            break;
        }
    }
    struct iNode iNode_block [num_files];
    read_sd_block(&iNode_block, 1);
    file_to_open->iNode_data = iNode_block[file_to_open->dir_entry.iNode_number];
    file_to_open->mode = mode;
    file_to_open->position = 0;
    file_to_open->next = NULL;
    add_to_open_files(file_to_open);
    return file_to_open;
}

File create_file(char *name, FileMode mode)
{
    if(!is_initialized())
    {
        initialize_fs();
    }
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
    struct DirectoryEntry dir_entry;
    int i = 2;
    for(; i< num_files +2; i++)
    {
        read_sd_block(&dir_entry, i);
        if(dir_entry.name[0] == 0) break;
    }
    if(i == NUM_INIT_BLOCKS)
    {
        fserror = FS_OUT_OF_SPACE;
        return NULL;
    }
    //i now used for inode number
    dir_entry.iNode_number = i -2;
    strncpy(dir_entry.name, name, 256);
    write_sd_block(&dir_entry, i);
    File file = malloc(sizeof(struct FileInternals));
    file->dir_entry = dir_entry;
    struct iNode iNode_block [num_files];
    read_sd_block(&iNode_block, 1);
    file->iNode_data = iNode_block[dir_entry.iNode_number];
    file->position =0;
    file->mode = mode;
    file->next = NULL;
    add_to_open_files(file);
    return file;
}
//debugging methods
void print_open_files()
{
    File file_iterator = first_open_file;
    if(!first_open_file)
    {
        printf("No open files \n");
    }
    while(file_iterator)
    {
        printf("File open with name %s inode number %d and length %llu \n", file_iterator->dir_entry.name, file_iterator->dir_entry.iNode_number, file_iterator->iNode_data.size);
        file_iterator = file_iterator->next;
    }
}

void print_file_data(File file )
{
    
}
void clearblock_test()
{
    char testblock[SOFTWARE_DISK_BLOCK_SIZE];
    memset(testblock, 'a', SOFTWARE_DISK_BLOCK_SIZE *sizeof(char));
    printf("%s\n",testblock);
    int blockNum = find_and_write_block(testblock);
    bzero(testblock, SOFTWARE_DISK_BLOCK_SIZE* (sizeof(char)));
    read_sd_block(testblock, blockNum);
    printf("%s written to block \n", testblock);
    clear_block(blockNum);
    read_sd_block(testblock, blockNum);
    printf("%s after clear_block\n", testblock);
}
void max_file_test()
{
    File fileArr[num_files];
    char  fileName[11];
   
    for(int i = 0; i < num_files ;i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        fileArr[i] = create_file(fileName, READ_ONLY);
        fs_print_error();
    }
    print_open_files();
    printf("closing all files...\n");
    for(int i = 0; i< num_files; i++)
    {
        close_file(fileArr[i]);
    }
    print_open_files();
    printf("opening all files ...\n");
    for(int i = 0; i < num_files ;i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        fileArr[i] = open_file(fileName, READ_ONLY);
        fs_print_error();
    }
    print_open_files();
    printf("closing all files...\n");
    for(int i = 0; i< num_files; i++)
    {
        close_file(fileArr[i]);
        fs_print_error();
    }
    print_open_files();
    printf("opening out of order... \n");
    for(int i = 0; i < num_files/2; i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        fileArr[i] = open_file(fileName, READ_ONLY);
        fs_print_error();
        sprintf(fileName, "%s%d%s","file",num_files - i -1,".txt");
        fileArr[num_files-i-1] = open_file(fileName, READ_ONLY);
        fs_print_error();
    }
    print_open_files();
    printf("closing out of order files \n");
    for(int i = 0; i< num_files; i++)
    {
        close_file(fileArr[i]);
        fs_print_error();
        print_open_files();
    }
    printf("trying to create an extra file \n");
    create_file("file17.txt", READ_ONLY);
    fs_print_error();
    printf("deleting files...\n");
    for(int i = 0; i < num_files; i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        delete_file(fileName);
        fs_print_error();
    }
}

void max_file_write_test(File file)
{
    char buf[MAX_FILE_SIZE];
    memset(buf, 'z', MAX_FILE_SIZE -1);
    buf[MAX_FILE_SIZE -1] = '\0';
    write_file(file, buf, MAX_FILE_SIZE-1);
    
    
    
}



int main(int argc, const char * argv[]) {
    // insert code here...
        printf("%ud\n", MAX_FILE_SIZE);
    init_software_disk();
    initialize_fs();
    File file = create_file("file.txt", READ_WRITE);
    close_file(file);
    file = open_file("file.txt", READ_WRITE);
    close_file(file);
    delete_file("file.txt");
    file = create_file("file.txt", READ_WRITE);
    //clearblock_test();
    //max_file_test();
   
    char test_data[515];
    memset(test_data, 'z', 515);
    test_data[514] = '\0';
    if(create_file("\0", READ_ONLY))
    {
        printf("Error should not have been created\n");
    }
    else
    {
        printf("File not created: Illegal File Name\n");
    }
    fs_print_error();
    max_file_write_test(file);
    seek_file(file, 1);
    unsigned long wrtn = write_file(file, test_data, 22);
    memset(test_data, 'a', 515);
    test_data[514] = '\0';
    seek_file(file, 0);
    //unsigned long wrtn2 = write_file(file, test_data, 514);
    unsigned long wrtn2 = write_file(file, "a", 1);
    fs_print_error();
    bzero(test_data, 515);
    read_sd_block(test_data, 18);
    write_file(file, "Hello World",12);
    printf("first block %s \n", test_data);
    bzero(test_data, 515);
    read_sd_block(test_data, 18);
    printf("second block %s \n", test_data);
    printf("size of file %llu \n",file->iNode_data.size);
    
    //create a lot of files
    printf("Creating lots of files/n");
    for(int i = 0; i < 25; i++){
        char buffer[50];
        sprintf(buffer, "file%d.txt", i);
        if(create_file(buffer, READ_WRITE))
            printf("file%d successfully created\n", i);
        else
            printf("file%d not created\n", i);
    }

    //delete file
    close_file(file);
    delete_file("file.txt");
    file = open_file("file.txt", READ_WRITE);
    fs_print_error();
    //check if now there is space to write another file
    file = create_file("file24.txt", READ_WRITE);
    fs_print_error();




    
    return 0;
}
