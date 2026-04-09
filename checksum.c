#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// this function gets the checksum output as an integer
int checksum (char str[]) {
    int temp = str[0];
    for (int i = 1; i < strlen(str); i++) {
        temp = temp^str[i];
    }
    return temp;
}

int extract (char input[])

int main () {
    int check; // this variable should be replaced by the checksum sent by previous STM

    char stringcheck[6];

    char input[100] = "helloB06_0B06_1B06_2B06_3"; // arbitrary string for testing (replace with message)
    //cast input to bytes

    
    char buffer[50]; // initialise buffer

    // extract previous checksum from message to check, then delete it from the input
    
        for (int i = strlen(input); i >= 0 ; i--) {
            // if statement clause idea, takes elements from this video: https://www.youtube.com/watch?v=p6uqGop26es&t=327s
            // 0x5F is the byte for a "_"
            if (strstr(&input[i], 0x5F) == &input[i]) {
                strcpy(stringcheck, &input[i+2]);
                check = atoi(&input[i+2]); 
                for (int j = i+2; j < i+2+strlen(stringcheck); j++)
                input[j] = input[j+strlen(stringcheck)];
                break;
            }
        }

    // perform checksum on message receieved
    int input_check = checksum(input);

    // compare with checksum received

    // if checksum doesn't match, turn light on and terminate code with return
    if (input_check != check) {
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
        return 1;
    // if checksum matches concatenate message and get new checksum
    } else {
        strcat(input, STM_ID);
        sprintf(buffer, "%d", checksum(input)); //converts checksum to string
        strcat(input, buffer);
    }
    // Add code to send message i.e. Transmit()

    return 0;
}