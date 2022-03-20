
// gcc main.c rs232.c -Wall -Wextra -o2 -o main
// .\main.exe

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "rs232.h"

int mbedDevice = 11; //device comPort
void readSerial(unsigned char* mbedInput){ //read mbed output char by char
    int n = 0;
    unsigned char c = ' ';

    while(c!='|'){ //uses delimeter '|' to know when mbed has finished sending message
        n += RS232_PollComport(mbedDevice, &c, 1); //stores char read in character
        mbedInput[n] = c; //character read is put into array
    }
    mbedInput[n] = '\0';
    printf("%s", mbedInput);  //guess is printed
}

int main(){
    int bdrate=9600;
    char mode[]={'8','N','1',0};
    unsigned char mbedInput[256];
    char pcOut[265];

    if(RS232_OpenComport(mbedDevice, bdrate, mode, 0)){ //attempt to connect to mbed
        printf("Can not open comport\n");
        return(0);
    }

    printf("\n----------------------------\n Welcome to my game!\n----------------------------\n Close guess:   -1 point\n Way-off guess: -2 points\n Invalid input: -10 points\n----------------------------\n");
    readSerial(mbedInput); //prompt to set secret number
    readSerial(mbedInput); //secret number has been set

    while (1){//loop will exit when user guesses number or score goes below 1

        readSerial(mbedInput); //ask for user guess
        scanf("%s",pcOut); //gets user guess
        fflush(stdin); //flush input stream
        strcat(pcOut,"\n"); //adds '\n' to end of string so mbed knows its the end of the string sent
        printf(" %s",pcOut);
        RS232_cputs(mbedDevice, pcOut); //sends string to mbed

        readSerial(mbedInput); //gets message to see if guess was: invalid, correct, close or way-off
        if(!strcmp((const char*)mbedInput," You guessed correctly!\n") || !strcmp((const char*)mbedInput," You ran out of guesses!\n")) //checks if message for guessing the secret number or message for guesses running out has been sent
            break;
    }

    printf("----------------------------\n");
    readSerial(mbedInput); //win/lose and score message
    RS232_CloseComport(mbedDevice); //close the port
    return(0);
}