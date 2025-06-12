#include <stdlib.h>

// simple hash function
// takes 32 bit character (0 at end)
// A = first byte, B = second byte
// hash = (A^B) ^ (A&B) ^ (A|B)


// int H(int* inputBuffer){
//     return inputBuffer[1] ^ inputBuffer[2] ^ inputBuffer[3] ^ inputBuffer[4];
// }

int H(unsigned int inputBuffer){
    return ((inputBuffer >> 24) & 255) ^ ((inputBuffer >> 16) & 255) ^ ((inputBuffer >> 8) & 255) ^ (inputBuffer & 255);
}