#include <stdio.h>
// #include "generate_key.h"




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





void func(char *input, char *key, char* output){
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

char* num_to_binary(int num){
    char *output = (char*)malloc(sizeof(char)*4);
    for(int i=0; i<4; i++) {
        output[3-i] = (num&1)+'0';
        num = num >> 1;
    }
    return output;
}
int binary_to_num(char* input){
    int out = 0;
    int exp = 1;
    for(int i=3; i>=0; i--){
        out += exp*((int)(input[i]-'0'));
        exp *= 2;
    }
    return out;
}




int isSame(char *s1, char *s2){
    for(int i=0; i<4; i++){
        if(s1[i]!=s2[i]) return 0;
    }
    return 1;
}

void print(char *str, int n){
    for(int i=0; i<n; i++) printf("%c", str[i]); printf(" ");
}

void DES_encrypt(char* input, char** keys, char* output){
    for(int i=0; i<8; i++) output[i] = input[i];
    
    char left[4], right[4], temp;
    for(int j=0; j<4; j++) {
        print(output, 8);
        // seperate left-right
        for(int i=0; i<4; i++) left[i] = output[i];
        for(int i=4; i<8; i++) right[i-4] = output[i];
        // pass right to function
        char func_out[6];
        func(right, keys[j], func_out);
        print(left, 4); print(right, 4); print(keys[j], 4); print(func_out, 4);
        // xor left and output of function
        int i=0;
        while(i<4) left[i] = ((left[i]-'0')^(func_out[i++]-'0'))+'0';
        // swap
        if(j<3){
            for(i=0; i<4; i++) output[i] = right[i];
            for(i=4; i<8; i++) output[i] = left[i-4];
        }
        else{
            for(i=0; i<4; i++) output[i] = left[i];
            for(i=4; i<8; i++) output[i] = right[i-4];
        }
        print(output, 8); printf("\n");
    }
}

void DES_decrypt(char* input, char** keys, char* output){
    for(int i=0; i<8; i++) output[i] = input[i];
    
    char left[4], right[4], temp;
    for(int j=3; j>=0; j--) {
        print(output, 8);
        // swap
        for(int i=0; i<4 && j<3; i++){
            temp = output[i];
            output[i] = output[i+4];
            output[i+4] = temp;
        }
        // seperate left-right
        for(int i=0; i<4; i++) left[i] = output[i];
        for(int i=4; i<8; i++) right[i-4] = output[i];
        // pass right to function
        char func_out[6];
        func(right, keys[j], func_out);
        print(left, 4); print(right, 4); print(keys[j], 4); print(func_out, 4);
        // xor left and output of function
        int i=0;
        while(i<4) left[i] = ((left[i]-'0')^(func_out[i++]-'0'))+'0';
        for(i=0; i<4; i++) output[i] = left[i];
        for(i=4; i<8; i++) output[i] = right[i-4];
        print(output, 8); printf("\n");
    }
}


int main(){
    // char **keys = gen_key(1234);
    // char input[8] = "10101001";
    // char output[8];
    // DES_encrypt(input, keys, output);
    // // for(int i=0; i<8; i++) printf("%c", input[i]); printf("\n");
    // for(int i=0; i<8; i++) printf("%c", output[i]); printf("\n");
    // DES_decrypt(output, keys, input);
    // for(int i=0; i<8; i++) printf("%c", input[i]); printf("\n");
    int input = 0, key = 0;
    char str[1024][4];
    int k = 0;
    char out[4];
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            func(num_to_binary(i), num_to_binary(j), out);
            for(int x = 0; x<k; x++){
                if(isSame(str[x], out)) printf("Same");
            }
            for(int x=0; x<4; x++) str[k][x]=out[x];
            k++;
        }
    }
    printf("Finish");
}