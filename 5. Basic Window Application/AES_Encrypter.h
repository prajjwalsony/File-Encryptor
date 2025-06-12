#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "data.h"
#include "generate_key.h"
#include "AES.h"
#include "hash.h"

double operate(FILE *readFile, FILE *writeFile, unsigned int key, int encrypt_decrypt){ 
    long long totalReadSize=0, totalWriteSize=0;
    double MB=0, GB=0;

    if(readFile==NULL || writeFile==NULL) return -7; //cannot encrypt or decrypt

    char *bytes = (char*)malloc(sizeof(char)*1024*1024);
    if(bytes==NULL) return -8; //cannot assign memory

    int **keys = genKey(key);
    if(keys==NULL) return -9; //there is problem with key generation
    int chunk[4];
    unsigned char temp;

    clock_t start = clock();

    int readSize=1;
    while(readFile && readSize){
        readSize = fread(bytes, sizeof(char), 1024*1024, readFile);
        if(readSize%4!=0){
            for(int j=readSize; j<(readSize + readSize%4); j++) bytes[j] = 0;
            readSize += readSize%4;
        }
        totalReadSize += readSize;
        if(encrypt_decrypt==-2){ //encrypt
            for(int i=0; i<readSize; i+=4){
                for(int j=0; j<4; j++){
                    temp = (unsigned char)bytes[i+j];
                    chunk[j] = (int)temp;
                }
                AES_encrypt(chunk, keys);
                for(int j=0; j<4; j++) bytes[i+j] = (signed char)chunk[j];
            }
        }
        else if(encrypt_decrypt>=0){
            for(int i=0; i<readSize; i+=4){
                for(int j=0; j<4; j++){
                    temp = (unsigned char)bytes[i+j];
                    chunk[j] = (int)temp;
                }
                AES_decrypt(chunk, keys);
                for(int j=0; j<4; j++) bytes[i+j] = (signed char)chunk[j];
            }
        }
        int writeSize = fwrite(bytes, sizeof(char), readSize, writeFile);
        totalWriteSize += writeSize;
        if(writeSize != readSize) return -10; //write an read bytes length are not same
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    free(bytes);
    return totalReadSize == totalWriteSize ? time_spent : -10; //write an read bytes length are not same
}

int extractKey(char *fileName){
    int N=0;
    char *c = fileName;
    while((*c)!='\0'){
        N++;
        c++;
    }
    char extension[8];
    extension[7]='\0';
    int n=6;

    c--;
    while((*c)!='.'){
        extension[n]=*c;
        n--;c--;N--;
        if(N==0) return -1; //invalid
    }
    if(strcmp("encrypt", extension)==0) {
        FILE *readFile = fopen(fileName, "rb");
        if(readFile==NULL) return -1;
        char byte; fread(&byte, sizeof(char), 1, readFile);
        int key = (int)byte;
        fclose(readFile);
        return key; //extracted key and have to decrypt
    }
    FILE *readFile = fopen(fileName, "rb");
    if(readFile==NULL) return -1;
    fclose(readFile);
    return -2; //have to encrypt
}






double AES(char *fileLocation, char pwd[4]){


    //key extraction and file verification
    int originalKey = extractKey(fileLocation); //Check Weather to encrypt of decrypt file and extract key
    if(originalKey==-1) return -1; //Invalid FIle Name OR Not able to open the file
    
    //pwd conversion and formate checking
    unsigned int enteredKey = 0;
    for(int i=0; i<4; i++) {
        if(pwd[i]<33 || 126<pwd[i]){
            return -2; //password formate incorrect
        }
    }
    for(int i=0; i<4; i++){
        enteredKey = enteredKey << 8;
        enteredKey += pwd[i]-'0';
    }

    //key verification
    if(originalKey>=0){
        if(originalKey!=H(enteredKey)) return 0; //Wrong Key
    }

    //Generate the new File Name
    char readFileName[500];
    strcpy(readFileName, fileLocation);
    char writeFileName[510];
    writeFileName[509]='\0';
    strcpy(writeFileName, readFileName);

    if(originalKey==-2) strcat(writeFileName, ".encrypt");
    else {
        char temp[510];
        int len =  strlen(writeFileName)-8;
        strncpy(temp, writeFileName, strlen(writeFileName)-7);
        temp[len]='\0';
        strcpy(writeFileName, temp);
    }

    //Opening files
    FILE *readFile = fopen(readFileName, "rb");
    if (readFile == NULL) return -3; //Not able to open input file

    FILE *writeFile = fopen(writeFileName, "wb");
    if (writeFile == NULL) {
        remove(writeFileName);
        return -4; //Not able to create output file
    }


    //Encrypting or decrypting file
    if(originalKey==-2) {
        int keyhash = H(enteredKey);
        int temp = fwrite(&keyhash, sizeof(char), 1, writeFile);  //have to encrypt
        if(!temp) return -5; //not able to write on output file
    }
    else {
        int temp = fread(&originalKey, sizeof(char), 1, readFile);  //have to decrypt
        if(!temp) return -6; //not able to read the input file
    }

    double N = operate(readFile, writeFile, enteredKey, originalKey);
    fclose(readFile); fclose(writeFile);

    if(N>=0) return N+1; //successful return time taken + 1 in seconds
    remove(writeFileName);
    return N; //error in operate function
}