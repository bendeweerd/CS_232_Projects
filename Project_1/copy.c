#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    //check for the correct number of arguments (3)
    if(argc != 3){
        perror("Error: incorrect number of arguments");
        return -1;
    }
    //check that the source file is a normal file
    
    //check the destination file doesn't already exist
    if(access(argv[2], F_OK) == 0){        
        perror("Error: destination file must not already exist");
        return -1;
    }

    //everything is ok so far, try to open the source file
    FILE *source;
    source = fopen(argv[1], "r");
    if(source == NULL){
        perror("Error: ");
        return -1;
    }
    
    //create destination file
    FILE *destination;
    destination = fopen(argv[2], "w");

    //copy the file one character at a time
    char c;
    c = fgetc(source);
    while(c != EOF){
        fprintf(destination, "%c", c);
        c = fgetc(source);
    }
    printf("\n\n");

    //done with the file; close it
    fclose(source);
    fclose(destination);

    return 0;
}

