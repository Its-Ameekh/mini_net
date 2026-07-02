#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,"Error: No string provided.\n");//so that it doesnt go to stdout just dev can see
        return 1; // Return 1 to tell the OS an error occurred.
    }

    for (int i = 1; i < argc; i++) {
        int count = 0;
        while (argv[i][count] != '\0') {
            unsigned char ch = (unsigned char)argv[i][count];
            count++; 

          
            for (int b = 7; b >= 0; b--) {
                
              
                int bit = (ch >> b) & 1;

                fprintf(stderr,"%d", bit);
            }

            fwrite(&ch,1,1,stdout);

           
            fprintf(stderr," ");
        }

        fprintf(stderr,"\n");
    }
    return 0; 
}