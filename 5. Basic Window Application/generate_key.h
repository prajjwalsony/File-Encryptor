//32 bit key
//8 bit word
//max 11 round
//32 bit round key

#include <stdlib.h>

#define uint unsigned int

int** genKey(unsigned int input){
    int words[44];
    words[0] = (input) >> 24;
    words[1] = (input >> 16) & 255;
    words[2] = (input >> 8) & 255;
    words[3] = (input) & 255;
    int i = 4;
    int word = words[3];
    for(i=4; i<44; i++){
        if(i%4==0) {
            int temp = subByte_table[word] ^ Rcon[i/4];
            word = words[i-4] ^ word;
        }
        else word = words[i-4] ^ word;
        words[i] = word;
    }
    int **keys = (int**)malloc(sizeof(int*)*11);
    i=0;
    int k = 0;
    while(i<44){
        keys[k] = (int*)malloc(sizeof(int)*4);
        for(int j=0; j<4; j++) keys[k][j] = words[i++];
        k++;
    }
    
    return keys;
}