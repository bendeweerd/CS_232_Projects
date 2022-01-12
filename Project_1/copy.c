#include <stdio.h>

int main(int argc, char *argv[]) {
    if(argc == 3){
        printf("Source: %s\n", argv[1]);
        printf("Destination: %s\n", argv[2]);
    }
    else {
        perror("Error: ");
        return -1;
    }
    return 0;
}

