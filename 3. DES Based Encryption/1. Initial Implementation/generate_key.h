#include <stdlib.h>

// key size = 14 bits {0000 - 16,383}
// input the key as 4 digit pin

char *rotate_left(char *input){
    char *output = (char*)malloc(sizeof(char)*14);
    char first = input[0];
    for(int i=0; i<13; i++) output[i] = input[i+1];
    output[13] = first;
    free(input);
    return output;
}

char *gen_round_key(char *input) {
    int x[14];
    for (int i = 0; i < 14; i++) x[i] = input[i] - '0';
    int y0 = x[3] ^ x[7] ^ (x[1] & x[2]) ^ (x[5] & x[6] & x[0]) ^ x[9];
    int y1 = x[4] ^ (x[0] & x[3]) ^ (x[12] & x[6]) ^ (x[2] & x[7] & x[8]) ^ x[13];
    int y2 = x[8] ^ (x[1] & x[5]) ^ (x[10] & x[11]) ^ (x[6] & x[12]) ^ x[3];
    int y3 = x[2] ^ (x[9] & x[10]) ^ (x[0] & x[11]) ^ (x[13] & x[5] & x[6]) ^ x[4];
    int y4 = x[0] ^ x[1] ^ (x[6] & x[7]) ^ (x[8] & x[9]) ^ (x[10] & x[11] & x[12]);
    int y5 = x[5] ^ x[6] ^ (x[2] & x[10]) ^ (x[3] & x[4] & x[13]) ^ x[1];
    char *output = (char*)malloc(sizeof(char) * 6);
    output[0] = y0 + '0';
    output[1] = y1 + '0';
    output[2] = y2 + '0';
    output[3] = y3 + '0';
    output[4] = y4 + '0';
    output[5] = y5 + '0';

    return output;
}

char* to_binary(int num){
    char *output = (char*)malloc(sizeof(char)*14);
    for(int i=0; i<14; i++) {
        output[13-i] = (num&1)+'0';
        num = num >> 1;
    }
    return output;
}

char** gen_key(int num){
    char **keys = (char**)malloc(sizeof(char*)*4);
    char *input = to_binary(num);
    for(int i=0; i<4; i++){
        input = rotate_left(input);
        keys[i] = gen_round_key(input);
    }
    free(input);
    return keys;
}