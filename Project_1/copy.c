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
        perror("Error: incorrect number of arguments.\nUsage: ./copy <src_file> <dest_file>\n");
        exit(EXIT_FAILURE);
    }
    
    src_filename = argv[1];
    dest_filename = argv[2];

    //check that the source file is a regular file
    //https://c-for-dummies.com/blog/?p=4101
    struct stat src_stat;
    int r;
    r = stat(src_filename, &src_stat);
    if(r == -1){
        perror("Error: unable to read source file.");
        exit(EXIT_FAILURE);
    }
    if(!S_ISREG(src_stat.st_mode)){
        perror("Error: source file not a regular file.");
        exit(EXIT_FAILURE);
    }
    
    //check the destination file doesn't already exist
    if(access(dest_filename, F_OK) == 0){        
        perror("Error: destination file must not already exist.");
        exit(EXIT_FAILURE);
    }

    //open the source file for reading only
    FILE *src_file;
    src_file = fopen(src_filename, "r");
    if(src_file == NULL){
        perror("Error: cannot open source file.");
        exit(EXIT_FAILURE);
    }
    
    //create destination file
    FILE *dest_file;
    dest_file = fopen(dest_filename, "w");
    if(dest_file == NULL){
        perror("Error: cannot create destination file.");
        exit(EXIT_FAILURE);
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

