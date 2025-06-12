#include <stdlib.h>

// key size = 14 bits {0000 - 16,383}
// input the key as 4 digit pin

int gen_round_key(int input){
    short int bits[14];
    for (short int i = 13; i >= 0; i--) {
        bits[i] = input & 1;
        input = input >> 1;
    }

    int y0 = bits[3] ^ bits[7] ^ (bits[1] & bits[2]) ^ (bits[5] & bits[6] & bits[0]) ^ bits[9];
    int y1 = bits[4] ^ (bits[0] & bits[3]) ^ (bits[12] & bits[6]) ^ (bits[2] & bits[7] & bits[8]) ^ bits[13];
    int y2 = bits[8] ^ (bits[1] & bits[5]) ^ (bits[10] & bits[11]) ^ (bits[6] & bits[12]) ^ bits[3];
    int y3 = bits[2] ^ (bits[9] & bits[10]) ^ (bits[0] & bits[11]) ^ (bits[13] & bits[5] & bits[6]) ^ bits[4];
    int y4 = bits[0] ^ bits[1] ^ (bits[6] & bits[7]) ^ (bits[8] & bits[9]) ^ (bits[10] & bits[11] & bits[12]);
    int y5 = bits[5] ^ bits[6] ^ (bits[2] & bits[10]) ^ (bits[3] & bits[4] & bits[13]) ^ bits[1];
    return (y5<<5) | (y4<<4) | (y3<<3) | (y2<<2) | (y1<<1) | (y0);
}

int *gen_key(int input){
    int * keys = (int*)(malloc(sizeof(int)*2));
    // ROR
    input = ((input&1)<<13) | (input>>1);
    keys[0] = gen_round_key(input);
    input = ((input&1)<<13) | (input>>1);
    keys[1] = gen_round_key(input);
    return keys;
}