/**********************************************
 * CS 232 Program 1 - C Copy Program
 * Ben DeWeerd
 * 1.13.2022
 * 
 * A simple C program to copy the contents of one
 * file to another.
 * 
 * Command line usage:
 * 
 *      ./copy <src_filename> <dest_filename>
**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *src_filename;
    const char *dest_filename;
    
    //check for the correct number of arguments
    if(argc != 3){
        fprintf(stderr, "Error: incorrect number of arguments.\n  Usage: ./copy <src_file> <dest_file>\n");
        exit(-1);
    }
    
    src_filename = argv[1];
    dest_filename = argv[2];

    //check that the source file is a regular file
    //https://c-for-dummies.com/blog/?p=4101
    struct stat src_stat;
    int r;
    r = stat(src_filename, &src_stat);
    if(r == -1){
        perror("Error: Unable to read source file");
        exit(-1);
    }
    if(!S_ISREG(src_stat.st_mode)){
        fprintf(stderr, "Error: Source file not a regular file.\n");
        exit(-1);
    }
    
    //check the destination file doesn't already exist
    if(access(dest_filename, F_OK) == 0){        
        fprintf(stderr, "Error: Destination file must not already exist.\n");
        exit(-1);
    }

    //open the source file for reading only
    FILE *src_file;
    src_file = fopen(src_filename, "r");
    if(src_file == NULL){
        perror("Error: Cannot open source file.");
        exit(-1);
    }
    
    //create destination file
    FILE *dest_file;
    dest_file = fopen(dest_filename, "w");
    if(dest_file == NULL){
        perror("Error: Cannot create destination file.");
        exit(-1);
    }

    //copy the file one character at a time
    //https://www.geeksforgeeks.org/c-program-print-contents-file/
    char c;
    c = fgetc(src_file);
    while(c != EOF){
        fputc(c, dest_file);
        c = fgetc(src_file);
    }

    //close files
    fclose(src_file);
    fclose(dest_file);

    //report success
    return 0;
}

