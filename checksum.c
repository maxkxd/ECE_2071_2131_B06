#include <stdio.h>
#include <string.h>

// this function gets the checksum output as an integer
int checksum (char str[]) {
    int temp = str[0];
    for (int i = 1; i < strlen(str); i++) {
        temp = temp^str[i];
    }
    return temp;
}

int main () {
    int check; // this variable should be replaced by the checksum sent by previous STM

    char stringcheck[6];

    char input[100] = "hello"; // arbitrary string for testing (replace with message)

    char buffer[50]; // initialise buffer

    // extract previous checksum from message to check, then delete it from the input
    
        for (int i = strlen(input); i > 0 ; i--) {
            // if statement clause idea, takes elements from this video: https://www.youtube.com/watch?v=p6uqGop26es&t=327s
            if (strstr(&input[i], "B06_3") == &input[i]) {
                strcpy(stringcheck, &input[i+5])
                check = atoi(&input[i+5]); 
                for (int j = i+5; j < strlen(check); j++)
                input[j] = input[j+strlen(check)];
            } else if (strstr(&input[i], "B06_2") == &input[i]) {
                strcpy(stringcheck, &input[i+5])
                check = atoi(&input[i+5]); 
                for (int j = i+5; j < strlen(check); j++)
                input[j] = input[j+strlen(check)];
            } else if (strstr(&input[i], "B06_1") == &input[i]) {
                strcpy(stringcheck, &input[i+5])
                check = atoi(&input[i+5]); 
                for (int j = i+5; j < strlen(check); j++)
                input[j] = input[j+strlen(check)];
            } else if (strstr(&input[i], "B06_0") == &input[i]) {
                strcpy(stringcheck, &input[i+5])
                check = atoi(&input[i+5]); 
                for (int j = i+5; j < strlen(check); j++)
                input[j] = input[j+strlen(check)];
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

// testing
    // printf("%d\n",test[0]^test[1]);
    // printf("%d\n",(test[0]^test[1])^test[2]);
    // printf("%d\n",((test[0]^test[1])^test[2])^test[3]);
    // printf("%d\n",(((test[0]^test[1])^test[2])^test[3])^test[4]);