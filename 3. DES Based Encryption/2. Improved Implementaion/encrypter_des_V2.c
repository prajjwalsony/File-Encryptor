#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fast_DES.h"
#include "generate_key.h"
#include "hash.h"

void erase(int n){ //erase the previous printed text in terminal
    for(int i=0; i<n; i++) printf("\b");
}

int operate(FILE *readFile, FILE *writeFile, int key, int encrypt_decrypt){ 
    long long totalReadSize=0, totalWriteSize=0;
    double MB=0, GB=0;

    if(readFile==NULL || writeFile==NULL) return 0;

    char *bytes = (char*)malloc(sizeof(char)*1024*1024*10);
    if(bytes==NULL) return 0;

    int *keys = gen_key(key);
    short int byte;
    unsigned char temp;

    clock_t start = clock();
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    long long it = 0;

    int readSize=1;
    while(readFile && readSize){
        readSize = fread(bytes, sizeof(char), 1024*1024*10, readFile);
        totalReadSize += readSize;
        if(encrypt_decrypt==-2){ //encrypt
            for(int i=0; i<readSize; i++){
                temp = (unsigned char)bytes[i];
                byte = (short int)temp;
                bytes[i] = (signed char)DES_encrypt(byte, keys[0], keys[1]);;
                // byte = DES_decrypt(byte, keys[0], keys[1]);
                // printf("%d %c\n", byte, (char)byte);
                
            }
        }
        else if(encrypt_decrypt>=0){
            for(int i=0; i<readSize; i++){
                temp = (unsigned char)bytes[i];
                byte = (short int)temp;
                bytes[i] = (signed char)DES_decrypt(byte, keys[0], keys[1]);
            }
        }
        int writeSize = fwrite(bytes, sizeof(char), readSize, writeFile);
        totalWriteSize += writeSize;
        if(writeSize != readSize) return 0;
        
        // if(it%10==0){
            MB = (double)totalReadSize / (1024 * 1024);
            GB = MB / 1024.0;
            end = clock();
            double speed = MB/((double)(end - start) / CLOCKS_PER_SEC);
            printf("%.0f MB  |  %.2f GB  |  Speed: %.2f MB/S", MB, GB, speed);
            erase(100);
        // }
        // it++;

    }
    printf("%.0f MB  |  %.2f GB\n", MB, GB);
    end = clock();
    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Total Time Taken: %.1f\n", time_spent);
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
            printf("Enter key(1000-9999) to encrypt this file: "); scanf("%d", &enteredKey);
            if(enteredKey>9999 || enteredKey<1000){
                printf("Enter the key in Valid range!!!");
                printf("\nPress Enter to exit!!!");
                int n; scanf("%d", &n); //input to hold the terminal
                return 1;
            }
        }
        else{
            printf("Enter key(1000-9999) to decrypt this file: "); scanf("%d", &enteredKey);
            if(enteredKey>9999 || enteredKey<1000){
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