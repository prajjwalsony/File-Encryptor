#include "generate_key.h"
#include "function.h"

char* xor_operation1(char *input1, char *input2){
    char *output = (char*)malloc(sizeof(char)*4);
    int i=0;
    while(i<4) output[i] = ((input1[i]-'0')^(input2[i++]-'0'))+'0';
    free(input1); free(input2); free(output);
    return output;
}

char* DES_round(char *input, char *round_key){
    char *output = (char*)malloc(sizeof(char)*8);
    char *left = (char*)malloc(sizeof(char)*4);
    char *right = (char*)malloc(sizeof(char)*4);
    for(int i=0; i<4; i++) left[i] = input[i];
    for(int i=4; i<8; i++) right[i-4] = input[i];

    left = xor_operation1(left, func(right, round_key));
    for(int i=0; i<4; i++) output[i] = left[i];
    for(int i=4; i<8; i++) output[i] = right[i-4];
    free(left); free(right); free(input);
    return output;
}

char* swapper(char* input){
    char *output = (char*)malloc(sizeof(char)*8);
    char *left = (char*)malloc(sizeof(char)*4);
    char *right = (char*)malloc(sizeof(char)*4);
    for(int i=0; i<4; i++) left[i] = input[i];
    for(int i=4; i<8; i++) right[i-4] = input[i];
    for(int i=0; i<4; i++) output[i] = right[i];
    for(int i=4; i<8; i++) output[i] = left[i-4];
    free(input); free(left); free(right);
    return output;
}


char *DES_encrypt(char* input, int key){
    char** keys = gen_key(key);
    char *output = input;
    for(int i=0; i<3; i++) output = swapper(DES_round(output, keys[i]));
    output = DES_round(output, keys[3]);
    free(keys); free(input);
    return output;
}

char *DES_decrypt(char* input, int key){
    char** keys = gen_key(key);
    char *output = input;
    for(int i=0; i<3; i++) output = swapper(DES_round(output, keys[3-i]));
    output = DES_round(output, keys[0]);
    free(keys); free(input);
    return output;
}