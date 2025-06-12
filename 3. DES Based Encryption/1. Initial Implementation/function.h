#include <stdlib.h>
// input 4 bit
// key input 6 bit
// output 4 bit


//4. straight p box
char* straight_p_box(char* input){
    char *output = (char*)malloc(sizeof(char)*4);
    int p_box[4] = {2, 1, 3, 0};
    for(int i=0; i<4; i++) output[i] = input[p_box[i]];
    free(input);
    return output;
}

//3. compresion s box
char* compression_s_box(char* input){
    char *output = (char*)malloc(sizeof(char)*4);
    short int inp[6];
    short int out[4];
    for(int i=0; i<6; i++) inp[i] = input[i]-'0';
    out[0] = ((!inp[2]) && inp[0]) || (inp[2] && (!inp[1])); //y1 = (¬x2 ∧ x0) ∨ (x2 ∧ ¬x1)
    out[1] = ((!inp[2]) && (!inp[0])) || (inp[2] && inp[1]); //y0 = (¬x2 ∧ ¬x0) ∨ (x2 ∧ x1)
    out[2] = (inp[4] && (!inp[3])) || (inp[5] && inp[3]);  //y1 = (x1 ∧ ¬x0) ∨ (x2 ∧ x0)
    out[3] = ((!inp[5]) && inp[4]) ^ inp[3];  //y0 = (¬x2 ∧ x1) ⊕ x0
    for(int i=0; i<4; i++) output[i] = out[i]+'0';
    free(input);
    return straight_p_box(output);
}

//2. xor
char* xor_operation(char *input, char *key){
    char *output = (char*)malloc(sizeof(char)*6);
    int i=0;
    while(i<6) output[i] = ((input[i]-'0')^(key[i++]-'0'))+'0';
    free(input); free(key);
    return compression_s_box(output);
}

//1. expansion p-box
char* expansion_p_box(char *input, char *key){
    char *output = (char*)malloc(sizeof(char)*6);
    output[0] = input[3];
    output[5] = input[0];
    int i = 1, j=0;
    for(int x=0; x<4; x++) output[i++] = input[j++];
    return xor_operation(output, key);
}


// initial function
char* func(char *input, char *key){
    return expansion_p_box(input, key);
}







