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
    char test[100] = "hello"; // sets an arbitrary test string
    printf("%d\n",test[0]^test[1]);
    printf("%d\n",(test[0]^test[1])^test[2]);
    printf("%d\n",((test[0]^test[1])^test[2])^test[3]);
    printf("%d\n",(((test[0]^test[1])^test[2])^test[3])^test[4]);
    int temp = checksum(test); // stores the return of the checksum into temp
    char buffer[50]; // initialise buffer
    sprintf(buffer, "%d", temp); // stores the integer as a string to buffer
    strcat(test, buffer); // concatenates buffer to the end of test string
    printf("%s\n",test); // prints the resulting test string

    return 0;
}