#include<stdio.h>
#include<string.h>
#include "filesystem.h"
#include "formatfs.h"


void read_full_file(File file)
{
    char buf[136704];
    bzero(buf,   1367043);
    
    read_file(file, buf, 136703);
    printf("full file %s \n", buf);
}


void max_file_test()
{
    File fileArr[16];
    char  fileName[11];
    
    for(int i = 0; i < 16 ;i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        fileArr[i] = create_file(fileName, READ_ONLY);
        fs_print_error();
    }
    printf("closing all files...\n");
    for(int i = 0; i< 16; i++)
    {
        close_file(fileArr[i]);
    }
    printf("opening all files ...\n");
    for(int i = 0; i < 16 ;i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        fileArr[i] = open_file(fileName, READ_ONLY);
        fs_print_error();
    }
    printf("closing all files...\n");
    for(int i = 0; i< 16; i++)
    {
        close_file(fileArr[i]);
        fs_print_error();
    }
    printf("opening out of order... \n");
    for(int i = 0; i < 16/2; i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        fileArr[i] = open_file(fileName, READ_ONLY);
        fs_print_error();
        sprintf(fileName, "%s%d%s","file",16 - i -1,".txt");
        fileArr[16-i-1] = open_file(fileName, READ_ONLY);
        fs_print_error();
    }
    printf("closing out of order files \n");
    for(int i = 0; i< 16; i++)
    {
        close_file(fileArr[i]);
        fs_print_error();
    }
    printf("trying to create an extra file \n");
    create_file("file17.txt", READ_ONLY);
    fs_print_error();
    printf("deleting files...\n");
    for(int i = 0; i < 16; i++)
    {
        sprintf(fileName, "%s%d%s","file",i,".txt");
        delete_file(fileName);
        fs_print_error();
    }
}

void max_file_write_test(File file)
{
    char buf[136704];
    memset(buf, 'z', 136703);
    buf[136703] = '\0';
    write_file(file, buf, 136703);
    
}

void read_test(File file)
{
    char * test_string = "Hello World";
    seek_file(file, 5);
    write_file(file, test_string, strlen(test_string));
    seek_file(file, 5);
    char rbuf[11];
    bzero(rbuf, 11);
    read_file(file, rbuf, 11);
    printf("read test %s \n", rbuf);
}


int main(int argc, const char * argv[]) {
    // insert code here...
    formatfs();
    max_file_test();
    File file = create_file("file.txt", READ_WRITE);
    read_test(file);
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
    seek_file(file, 0);
    read_full_file(file);
    close_file(file);
    delete_file("file.txt");
    
    /*seek_file(file, 1);
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
     fs_print_error();*/
    
    
    
    
    
    return 0;
}

