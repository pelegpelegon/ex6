/*
Name: Peleg Sacher
ID: 333135440
Assignment: ex6
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define END_OF_STRING '\0'


int getInt(const char* prompt){
    int success = 0, input;
    printf("%s", prompt);

    success = scanf(" %d", &input);
    while (!success){
        printf("\nInvalid input: ");
        success = scanf(" %d", &input);
    }
    printf("\n");
    return input;
}


char* getString(const char* prompt){
    char currChr, *pString = NULL;
    int index = 1;

    printf("%S", prompt);

    /* handles user not entering anything */
    do{
        scanf("%c[^\n]", &currChr);
    }while(currChr == '\n');

    /* dynamically increase string size to get the full string */
    while(currChr != '\n'){
        pString = realloc(pString, (index + 1) * sizeof(char));

        if(pString == NULL)
            exit(1);
             
        pString[index - 1] = currChr;
        pString[index] = END_OF_STRING;
        index++;   

        currChr = getchar();
    }

    printf("\n");
    return pString;   
}
