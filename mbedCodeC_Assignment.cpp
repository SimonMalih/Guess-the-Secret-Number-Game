#include "mbed.h"
#include "TextLCD.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

TextLCD lcd(p15, p16, p17, p18, p19, p20);
Serial pc(USBTX, USBRX); // tx, rx
BusOut rows(p26,p25,p24);
BusIn cols(p14,p13,p12,p11);

int secretNumber = -1;
int score = 100;

char invalid[] = {'F', 'E', 'D', 'A', 'B'};
int length = sizeof(invalid)/ sizeof(invalid[0]);
char Keytable[] = { 'F', 'E', 'D', 'C',
                    '3', '6', '9', 'B',
                    '2', '5', '8', '0',
                    '1', '4', '7', 'A'
};

char getKey(){
    int i,j;
    char ch=' ';
    for (i = 0; i <= 3; i++) {
        rows = i;
        for (j = 0; j <= 3; j++) {
            if (((cols ^ 0x00FF) & (0x0001<<j)) != 0) {
                ch = Keytable[(i * 4) + j];
            }
        }
    }
    return ch;
}

bool invalidInput(char key){ //return false if invalid key is entered
    for(int i = 0; i<length; i++)
        if(key == invalid[i])
            return false;
        return true;
}

bool isDigit(char key){ //returns false if key is not a number
    if(key>=48 && key<=57){
        return true;
    } else
        return false;
}

bool scoreLeft(){ //returns true if the users score is above 0
    if(score<=0){
        pc.printf("You ran out of guesses!\n|");
        score = 0;
        return true;
    }
    return false;
}

void clear(){ //clears screen, re-writes secret number and repositions lcd starting location
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Secret Num: %d", secretNumber);
    lcd.locate(0,1);
}

void userInput(char* input){ //store userinput from mbed keypad to input array
    int tracker = 0; //keeps track of position in the array
    char key = ' ';

    while(tracker<length-1) { //while loop will stop if 'C' is entered or input length exceeds 9
        key = getKey();
        if(key != ' '){
            if(key == 'C'){ //pressing enter will end the char array and the while loop
                input[tracker] = '\0';
                break;
            } else if(invalidInput(key)){ //if key entered is valid then the char is added to the char array
                lcd.printf("%c",key);
                wait(0.3);
                input[tracker] = key; //putting key into array
                tracker++;
            }
        }
    }
}

int stringToint (char arr[]){ // if user input is invalid int returns -1 else string converted to an int is returned
    for(int i = 0;arr[i]!='\0' ; i++){
        if(arr[i] == '\n'){
            arr[i] ='\0'; //replaces '\n' to '\0'
        }
    }

    int number = -1;
    int arrLen = (int) strlen(arr);

    for(int i = 0; i<arrLen; i++){
        if(!isDigit(arr[i])){
            return -1; //return -1 if char is not a digit
        }
    }

    number = atoi(arr);
    if(number > 30)
        return -1; //return -1 if number is bigger than 30//
        else
            return number; //returns string arr converted to int
}

int within(int value, int goal, int n){ //checks if value is within n range of goal
    int difference = goal-value;

    if(difference<0)
        difference*=-1;
    if(difference<=n)
        return 1;
    else
        return 0;
}

int distance(int guess){ //returns distance from guess to secret number
    for(int i = 1;i<5; i++){
        if(guess+i == secretNumber || guess-i == secretNumber)
            return i;
    }
}

int main() {

    int inputLength = 10; //max user input length
    char secretInput[inputLength];

    pc.printf("Please set secret number!\n|");

    //-1 == invalid input
    while(secretNumber==-1){
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Secret Num: ");
        userInput(secretInput); //gets user input
        secretNumber = stringToint(secretInput); //converts to int
        if(secretNumber==-1){ //if userinput is invalid
            secretNumber = -1;
            lcd.locate(0,1);
            lcd.printf("Invalid!");
            wait(0.7);
        } else
            break; //break if user input is valid
    }
    pc.printf("Secret number has been set!\n----------------------------\n|");
    clear(); //set lcd

    char userInput[256];
    int guess = 0;

    while(1){
        pc.printf("What is your guess? |");
        pc.gets(userInput, 256); //gets user guess from pc
        guess = stringToint(userInput); //converts the guess to an integer
        clear();

        if(guess == -1){//checks if user guess is invalid
            score -=10; //deduct 10 points for invalid input
            clear();
            if(scoreLeft()) //check if score is bigger than 0
                break;
            pc.printf("Invalid Input!\n|");
        } else if(guess == secretNumber){ //checks if guess is correct
            lcd.printf("%s",userInput); //prints the guess to the lcd screen
            pc.printf("You guessed correctly!\n|");
            break;
        } else {
            lcd.printf("%s",userInput); //prints the guess to the lcd screen
            score--; //score is decremented for incorrect guess
            if(scoreLeft()) //check if score is bigger than 0
                break;
            if(within(guess, secretNumber, 4)) //if guess is close to secretNumber it will say how far away you are
                pc.printf("You are less than %d away!\n|", distance(guess)+1);
            else {
                score--; //decrement score again since the guess is way-off
                if(scoreLeft())
                    break;
                pc.printf("You are way off.\n|");
            }
        }
    }

    if(score<=0) //check if user ran out of guesses
        pc.printf("You lost!\n Your score is 0\n----------------------------|");
    else
        pc.printf("You Won!\n Your score is %d\n----------------------------|",score);

    clear();

    for(int i = 0; i<8; i++){ //GAME OVER will scroll across the display
        clear();
        lcd.locate(i,1);
        lcd.printf("GAME OVER");
        wait(0.5);
    }

    clear();
    lcd.printf("Score: %d",score); //score is printed
    wait(0.3);
}