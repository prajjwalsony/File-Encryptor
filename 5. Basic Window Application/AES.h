//32 bit input
//32 bit key input
//32 bit output

#include <stdlib.h>

#define uint unsigned int

void round1(int *buffer, int *keyBuffer){ //4 integers of 8 bits
    //subByte
    for(int i=0; i<4; i++) buffer[i] = subByte_table[buffer[i]];
    //shiftRow
    int temp = buffer[1];
    buffer[1] = buffer[3];
    buffer[3] = temp;
    //mixColumn is removed
    //xor with key
    for(int i=0; i<4; i++) buffer[i] = buffer[i] ^ keyBuffer[i];
}

void inv_round(int *buffer, int *keyBuffer){ //4 integers of 8 bits
    //shiftRow
    int temp = buffer[1];
    buffer[1] = buffer[3];
    buffer[3] = temp;
    //invSubByte
    for(int i=0; i<4; i++) buffer[i] = invSubByte_table[buffer[i]];
    //mixColumn is removed
    //xor with key
    for(int i=0; i<4; i++) buffer[i] = buffer[i] ^ keyBuffer[i];
}

//3 round
void AES_encrypt(int *inputBuffer, int **keys){//4 integers of 8 bits in input and 4 keys for 3+1 round;
    //xor at the start
    for(int i=0; i<4; i++) inputBuffer[i] = inputBuffer[i] ^ keys[0][i];
    // round 1
    round1(inputBuffer, keys[1]);
    // round2
    round1(inputBuffer, keys[2]);
    // round3
    round1(inputBuffer, keys[3]);
}

//3 round
void AES_decrypt(int *inputBuffer, int **keys){//4 integers of 8 bits in input and 4 keys for 3+1 round;
    //xor at the start
    for(int i=0; i<4; i++) inputBuffer[i] = inputBuffer[i] ^ keys[3][i];
    // round 1
    inv_round(inputBuffer, keys[2]);
    // round2
    inv_round(inputBuffer, keys[1]);
    // round3
    inv_round(inputBuffer, keys[0]);
}