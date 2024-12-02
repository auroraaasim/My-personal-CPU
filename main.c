#include <stdio.h>
#include <stdlib.h>
#include "xtra.h"

int main(int argc, char **argv) {
    FILE *file;


    // opening the file in binary mode for reading
    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

//passing the file handle to the translate function made in xtra.c class
    translate(file);



    // closing the file
    fclose(file);

    return 0;
}
