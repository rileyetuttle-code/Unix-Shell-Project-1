//Riley Tuttle
//Dane Christisen

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// header files that dont exist on windows, but will run in linux
#include <unistd.h> 
#include <sys/wait.h>

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
        {
            exit(0);
        }

        //Start of string seperator
        char *delimiter =  " \n";
        char *rest = line; // done to leave line to be used in getline because strsep moves whatever pointer given to it
        int i = 0;
        char *word = strsep(&rest, delimiter);

        while(word != NULL)
        {
            if(word[0] != '\0') // skip empty spaces
            {
                cmd[i] = word;
                i++;
            }

            word = strsep(&rest, delimiter);
        }
        cmd[i] = NULL; // last spot in the list ends with NULL to satisfy execvp

        // built in exit command


        // built in cd command


        // build in path command

        //Fork and run the program
        int p1 = fork();
        if(p1 == 0){ // only child runs the program 
            execv(cmd[0], cmd);
            printf("No command found\n");
            exit(1); // child exits after command runs
        }
        //parent waits for program to finish
        wait(NULL);
    
    }


}
