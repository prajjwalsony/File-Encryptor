#include <stdlib.h>

// simple hash function
// takes 16 bit character (0 at end)
// A = first byte, B = second byte
// hash = (A^B) ^ (A&B) ^ (A|B)

int* to_num(int num){
    int left = 0, right = 0;
    int exp = 1;
    for(int i=0; i<8; i++){
        right += ((num&1)*exp);
        num = num >> 1;
        exp *= 2;
    }
    exp = 1;
    for(int i=0; i<8; i++){
        left += ((num&1)*exp);
        num = num >> 1;
        exp *= 2;
    }
    int *out = (int*)malloc(sizeof(int)*2);
    out[0] = left;
    out[1] = right;
    return out;
}


int H(int input){
    int *nums = to_num(input);
    int A = nums[0], B = nums[1];
    return A^B;
}