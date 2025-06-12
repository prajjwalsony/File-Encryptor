#include <stdlib.h>
// input 4 bit
// key input 6 bit
// output 4 bit

// initial function
void func(char *input, char *key, char* output){
    // char *output = (char*)malloc(sizeof(char)*6);
    // expansion p-box
    output[0] = input[3];
    output[5] = input[0];
    int i = 1, j=0;
    for(int x=0; x<4; x++) output[i++] = input[j++];
    // XOR
    i=0;
    while(i<6) output[i] = ((output[i]-'0')^(key[i++]-'0'))+'0';
    // compresion s box
    short int inp[6];
    short int out[4];
    for(int i=0; i<6; i++) inp[i] = output[i]-'0';
    out[0] = ((!inp[2]) && inp[0]) || (inp[2] && (!inp[1])); //y1 = (¬x2 ∧ x0) ∨ (x2 ∧ ¬x1)
    out[1] = ((!inp[2]) && (!inp[0])) || (inp[2] && inp[1]); //y0 = (¬x2 ∧ ¬x0) ∨ (x2 ∧ x1)
    out[2] = (inp[4] && (!inp[3])) || (inp[5] && inp[3]);  //y1 = (x1 ∧ ¬x0) ∨ (x2 ∧ x0)
    out[3] = ((!inp[5]) && inp[4]) ^ inp[3];  //y0 = (¬x2 ∧ x1) ⊕ x0
    for(int i=0; i<4; i++) output[i] = out[i]+'0';
    // straight p box
    char temp[4];
    int p_box[4] = {2, 1, 3, 0};
    for(int i=0; i<4; i++) temp[i] = output[i];
    for(int i=0; i<4; i++) output[i] = temp[p_box[i]];
}







