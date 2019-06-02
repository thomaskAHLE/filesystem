#include<stdio.h>
#include<string.h>
#include "filesystem.h"
#include "formatfs.h"


void read_full_file(File file)
{
    char buf[get_max_file_size()];
    bzero(buf, get_max_file_size());
    
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

	printf("printing all fliles \n");
	list_files();
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
    char buf[get_max_file_size()];
    memset(buf, 'z', get_max_file_size());
    buf[get_max_file_size()] = '\0';
    write_file(file, buf, get_max_file_size());
    
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
    printf("max file write test\n");
    max_file_write_test(file);
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
	return 0;
}

