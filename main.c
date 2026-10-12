//Riley Tuttle
//Dane Christisen
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include<string.h>

int main(){
    //Input string
    char InputString[50];

    //Main while loop
     while (true){
        printf("myshell> "); // basic shell prompt

        //Exit command
        if (strcmp(InputString, "exit") == 1){
            return 0;
        }

        getline(&InputString, stdin); // getline to read in the command from user


    }
}
