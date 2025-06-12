#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DES.h"
#include "hash.h"

char* num_to_binary(int num){
    char *output = (char*)malloc(sizeof(char)*8);
    for(int i=0; i<8; i++) {
        output[7-i] = (num&1)+'0';
        num = num >> 1;
    }
    return output;
}
int binary_to_num(char* input){
    int out = 0;
    int exp = 1;
    for(int i=7; i>=0; i--){
        out += exp*((int)(input[i]-'0'));
        exp *= 2;
    }
    free(input);
    return out;
}

void erase(int n){ //erase the previous printed text in terminal
    for(int i=0; i<n; i++) printf("\b");
}

int operate(FILE *readFile, FILE *writeFile, int key, int encrypt_decrypt){ 
    long long totalReadSize=0, totalWriteSize=0;
    double MB=0, GB=0;

    if(readFile==NULL || writeFile==NULL) return 0;

    char *bytes = (char*)malloc(sizeof(char)*1024*1024);
    if(bytes==NULL) return 0;

    int readSize=1;
    while(readFile && readSize){
        readSize = fread(bytes, sizeof(char), 1024*1024, readFile);
        totalReadSize += readSize;
        if(encrypt_decrypt==-2){
            for(int i=0; i<readSize; i++) {
                // printf("%d %c ", (int)bytes[i], bytes[i]);
                bytes[i] = (char)binary_to_num(DES_encrypt(num_to_binary((int)bytes[i]), key));
                // printf("%d %c\n",binary_to_num(DES_decrypt(num_to_binary((int)bytes[i]), key)), binary_to_num(DES_decrypt(num_to_binary((int)bytes[i]), key)));
            }
        }
        else if(encrypt_decrypt>=0){
            for(int i=0; i<readSize; i++) {
                bytes[i] = (char)binary_to_num(DES_decrypt(num_to_binary((int)bytes[i]), key));
            }
        }
        int writeSize = fwrite(bytes, sizeof(char), readSize, writeFile);
        totalWriteSize += writeSize;
        if(writeSize != readSize) return 0;
        
        MB = (double)totalReadSize / (1024 * 1024);
        GB = MB / 1024.0;
        printf("%.0f MB  |  %.2f GB", MB, GB);
        erase(50);

    }
    printf("%.0f MB  |  %.2f GB\n", MB, GB);
    free(bytes);
    // printf("%d %d\n", totalReadSize, totalWriteSize);
    return totalReadSize == totalWriteSize;
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
    return -2; //have to encrypt
}




int main(int argn, char *args[]){

    if(argn<2){  //Check for any file name passed as argument
        char *name = (char*)malloc(50);
        printf("Enter the file name: ");
        scanf("%s",name);
        args[1]=name;
    }


    //key extraction and verification
    int originalKey = extractKey(args[1]); //Check Weather to encrypt of decrypt file and extract key
    int enteredKey = -1;

    if(originalKey==-1){
        printf("Invalid FIle Name!!");
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    else{
        if(originalKey==-2){ //take the input for key
            printf("Enter key(1-255) to encrypt this file: "); scanf("%d", &enteredKey);
            if(enteredKey>9999 || enteredKey<0){
                printf("Enter the key in Valid range!!!");
                printf("\nPress Enter to exit!!!");
                int n; scanf("%d", &n); //input to hold the terminal
                return 1;
            }
        }
        else{
            printf("Enter key(1-255) to decrypt this file: "); scanf("%d", &enteredKey);
            if(enteredKey>9999 || enteredKey<0){
                printf("Enter the key in Valid range!!!");
                printf("\nPress Enter to exit!!!");
                int n; scanf("%d", &n); //input to hold the terminal
                return 1;
            }
            if(originalKey!=H(enteredKey)){
                printf("Wrong Key!!");
                printf("\nPress Enter to exit!!!");
                int n; scanf("%d", &n); //input to hold the terminal
                return 1;
            }
        }
    }

    //Generate the new File Name
    char readFileName[500];
    strcpy(readFileName, args[1]);
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
    printf("Processing...\n");
    erase(15);
    FILE *readFile = fopen(readFileName, "rb");
    if (readFile == NULL) {
        printf("Not able to open this file: %s", readFileName);
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }

    FILE *writeFile = fopen(writeFileName, "wb");
    if (writeFile == NULL) {
        if(originalKey==-2){
            printf("There is Error during creating a decrypted file");
        }
        else{
            printf("There is Error during creating a encrypted file");
        }
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        remove(writeFileName);
        return 1;
    }


    //Encrypting or decrypting file
    int a=-1;
    if(originalKey==-2) {
        int keyhash = H(enteredKey);
        a=fwrite(&keyhash, sizeof(char), 1, writeFile);  //have to encrypt
    }
    else {
        a=fread(&originalKey, sizeof(char), 1, readFile);  //have to decrypt
    }

    int N = operate(readFile, writeFile, enteredKey, originalKey);
    fclose(readFile); fclose(writeFile);

    if(N && originalKey!=-2) printf("Successfully Decrypted!!!");
    else if(N && originalKey==-2) printf("Successfully Encrypted!!!");
    else{
        printf("Cannot Encrypt/Decrypt File");
        remove(writeFileName);
    }


    printf("\nPress Enter to exit!!!\n");
    int n; scanf("%d", &n); //input to hold the terminal
    return 1;
}