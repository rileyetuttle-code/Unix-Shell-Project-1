//Riley Tuttle
//Dane Christisen
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(){
    char InputString[50]; //Store user inputted string
    char *cmd[64]; // array of pointers to commands in input line
    char *line = NULL; // char array pointer to recieve command input temporarily
    size_t cap = 0; // keeps track of how big cmd is that is read in from getline

    //Main while loop
     while (true){
        printf("myshell> "); // basic shell prompt

        //Exit command
        if (strcmp(InputString, "exit") == 1){
            return 0;
        }

        // input
        getline(&InputString, stdin); // getline to read in the command from user

        


        // parse input into commands



        // execute commands using forks


    }
}
