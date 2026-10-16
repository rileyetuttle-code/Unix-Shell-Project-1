//Riley Tuttle
//Dane Christisen

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(void) {
    
    bool done = false; // bool variable to control exiting loop
    char *cmd[64]; // array of pointers to commands in input line
    char *line = NULL; // char array pointer to recieve command input temporarily
    size_t cap = 0; // keeps track of how big cmd is that is read in from getline

    // complete external loop that will end when exit bool variable turns to true
    while(done == false)
    {
        printf("myshell> ");

        int numChars = getline(&line, &cap, stdin); // getline fills the line char array with the "string" from input and returns the number of characters in the array into our "numChars" variable
        if(numChars < 0) // check for user using ctrl d to exit program so input returns -1
            done = true;

        //Start of string seperator
        char delimiter = ' ';
        char *strsep(line, delimiter);

        //Fork and run the program
        fork();
        if(fork() != 0){
            execv(strsep);
            printf("No command found");
        }
        //wait for program to finish
        wait();
    




    }


}
