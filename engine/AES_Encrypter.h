/*
UPDATE HISTORY:
16/02/2026 - first made

-------------------------------------------------------------------------------

DEPENDENCIES:
standard - stdlib.h, string.h, time.h, windows.h
non standard - data.h, generate_key.h, AES.h, hash.h

-------------------------------------------------------------------------------

FILE DESCRIPTION:
AES_Encryptes.h provide multithreaded encryption api 
AES(char *fileLocation, char pwd[4]) take file location and password sting, 
encrypt/decrypt and return int status value as per success or error
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "data.h"
#include "generate_key.h"
#include "AES.h"
#include "hash.h"

struct ThreadArgs{
    unsigned int key;
    int encrypt_decrypt;
    int *returnValue;
    int threadIndex;
};

// Configuration
const int BUFFER_SIZE = 1024*1024*10; //in bytes 10MB
#define THREAD_NO 11

// output information
unsigned long long processedFileSize = 0; //can be accessed outside
short int IS_COMPLETED = 0;
unsigned long long SPEED = 0; // in bytes per second

FILE *readFilePointer;
FILE *writeFilePointer;

CRITICAL_SECTION processedFileSizeMutex;
CRITICAL_SECTION readFileAccessMutex[THREAD_NO];
CRITICAL_SECTION writeFileAccessMutex[THREAD_NO];

int takeKeyInput(unsigned int *inputKey){
    unsigned char key[4];
    printf("Enter key(4-character) to encrypt this file: ");
    for(int i=0; i<4; i++) {
        scanf(" %c", &key[i]);
        if(key[i]<33 || 126<key[i]){
            return 0; //error
        }
    }
    *inputKey = 0;
    for(int i=0; i<4; i++){
        *inputKey = *inputKey << 8;
        *inputKey += key[i]-'0';
    }
    return 1; //success
}


DWORD WINAPI worker(LPVOID arg){
    //extract arguments
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    unsigned int key = args->key;
    int encrypt_decrypt = args->encrypt_decrypt;
    int *returnValue = args->returnValue;
    int threadIndex = args->threadIndex;

    //assign memory
    char *bytes = (char*)malloc(sizeof(char)*BUFFER_SIZE);
    if(bytes==NULL){
        *returnValue = -8;
        return -8; //cannot assign memory
    }
    
    //generate keys
    int **keys = genKey(key);
    if(keys==NULL){
        *returnValue = -9;
        return -9; //there is problem with key generation
    }

    int chunk[4];
    unsigned char temp;
    int readSize;
    while(1){
        //read file and pass token
        EnterCriticalSection(&readFileAccessMutex[threadIndex]);
        readSize = fread(bytes, sizeof(char), BUFFER_SIZE, readFilePointer);
        LeaveCriticalSection(&readFileAccessMutex[(threadIndex+1)%THREAD_NO]);

        //parallel encryption or decryption
        if(readSize==0){
            *returnValue = 1;
            return 1; //success
        }
        if(readSize%4!=0){
            for(int j=readSize; j<(readSize + (4-readSize%4)); j++) bytes[j] = 0;
            readSize += 4 - readSize%4;
        }
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
        else if(encrypt_decrypt>=0){ //decrypt
            for(int i=0; i<readSize; i+=4){
                for(int j=0; j<4; j++){
                    temp = (unsigned char)bytes[i+j];
                    chunk[j] = (int)temp;
                }
                AES_decrypt(chunk, keys);
                for(int j=0; j<4; j++) bytes[i+j] = (signed char)chunk[j];
            }
        }

        //write and pass token
        EnterCriticalSection(&writeFileAccessMutex[threadIndex]);
        int writeSize = fwrite(bytes, sizeof(char), readSize, writeFilePointer);
        LeaveCriticalSection(&writeFileAccessMutex[(threadIndex+1)%THREAD_NO]);

        if(writeSize != readSize){
            *returnValue = -10;
            return -10; //write an read bytes length are not same
        }

        //lock and update totalProcessedSize
        EnterCriticalSection(&processedFileSizeMutex);
        processedFileSize += readSize;
        LeaveCriticalSection(&processedFileSizeMutex);
    }
}

double operate(FILE *readFile, FILE *writeFile, unsigned int key, int encrypt_decrypt){ 
    //mutex initialization
    InitializeCriticalSection(&processedFileSizeMutex);
    for(int i=0; i<THREAD_NO; i++){
        InitializeCriticalSection(&readFileAccessMutex[i]);
        InitializeCriticalSection(&writeFileAccessMutex[i]);
    }
    for(int i=1; i<THREAD_NO; i++){
        EnterCriticalSection(&readFileAccessMutex[i]);
        EnterCriticalSection(&writeFileAccessMutex[i]);
    }
    
    if(readFile==NULL || writeFile==NULL){
        return -7; //cannot encrypt or decrypt
    }
    
    readFilePointer = readFile;
    writeFilePointer = writeFile;
    
    //thread initialization and running
    struct ThreadArgs args[THREAD_NO];
    HANDLE threads[THREAD_NO];
    clock_t start = clock();
    for (int i = 0; i < THREAD_NO; i++) {
        args[i].key = key;
        args[i].encrypt_decrypt = encrypt_decrypt;
        args[i].returnValue = (int*)malloc(sizeof(int));
        *(args[i].returnValue) = 2; //initial value
        args[i].threadIndex = i;
        
        threads[i] = CreateThread(
            NULL,
            0,
            worker,
            &args[i],
            0,
            NULL
        );
    }

    WaitForMultipleObjects(THREAD_NO, threads, TRUE, INFINITE);
    clock_t end = clock();

    // speed calculation
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    SPEED = processedFileSize/time_spent;
    
    
    for (int i = 0; i < THREAD_NO; i++) CloseHandle(threads[i]);
    
    for(int i=0; i<THREAD_NO; i++){
        if ((*(args[i].returnValue)) != 1){ //any thread didn't return success (==1)
            return (*(args[i].returnValue));
        }
    }

    return 1 ; //success;
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
    if(originalKey==-1) {
        IS_COMPLETED = 1;
        return -1; //Invalid FIle Name OR Not able to open the file
    }
    
    //pwd conversion and formate checking
    unsigned int enteredKey = 0;
    for(int i=0; i<4; i++) {
        if(pwd[i]<33 || 126<pwd[i]){
            IS_COMPLETED = 1;
            return -2; //password formate incorrect
        }
    }
    for(int i=0; i<4; i++){
        enteredKey = enteredKey << 8;
        enteredKey += pwd[i]-'0';
    }

    //key verification
    if(originalKey>=0){
        if(originalKey!=H(enteredKey)){
            IS_COMPLETED = 1;
            return 0; //Wrong Key
        }
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
    if (readFile == NULL){
        IS_COMPLETED = 1;
        return -3; //Not able to open input file
    }

    FILE *writeFile = fopen(writeFileName, "wb");
    if (writeFile == NULL) {
        remove(writeFileName);
        IS_COMPLETED = 1;
        return -4; //Not able to create output file
    }

    //Encrypting or decrypting file
    if(originalKey==-2) {
        int keyhash = H(enteredKey);
        int temp = fwrite(&keyhash, sizeof(char), 1, writeFile);  //have to encrypt
        if(!temp){
            IS_COMPLETED = 1;;
            return -5; //not able to write on output file
        }
    }
    else {
        int temp = fread(&originalKey, sizeof(char), 1, readFile);  //have to decrypt
        if(!temp){
            IS_COMPLETED = 1;;
            return -6; //not able to read the input file
        }
    }

    double N = operate(readFile, writeFile, enteredKey, originalKey);
    fclose(readFile); fclose(writeFile);
    IS_COMPLETED = 1;

    if(N>=0) return N+1; //successful return time taken + 1 in seconds
    remove(writeFileName);
    return N; //error in operate function
}


// int main(){
//     char *name = (char*)malloc(50);
//     printf("Enter the file name: ");
//     scanf("%s", name);

//     char pwd[5];
//     printf("Enter key: ");
//     scanf("%s", &pwd);
//     printf("Result: %d", AES(name, pwd));
// }