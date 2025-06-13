#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long getFileSize(FILE *file) {
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return fileSize;
}

void erase(int n){ //erase the previous printed text in terminal
    for(int i=0; i<n; i++) printf("\b");
}

int operate(FILE *readFile, FILE *writeFile, int key, int totalReadSize){
    printf("size: %d\n", totalReadSize);

    if(readFile==NULL || writeFile==NULL) return -1;

    char *bytes = (char*)malloc(sizeof(char)*1024*1024);
    if(bytes==NULL) return -2;

    // printf("Completed: ");
    int total=0, preDigit=0;

    int readSize=1;
    while(readFile && readSize){
        readSize = (fread(bytes, sizeof(char), 1024*1024, readFile));
        for(int i=0; i<readSize; i++) bytes[i] = bytes[i] ^ key;
        int writeSize = fwrite(bytes, sizeof(char), readSize, writeFile);
        if(writeSize != readSize) return -3;

        total += readSize;
        int percent = (total*100)/totalReadSize;
        erase(100);
        printf("%d", total/(1024*1024));
        // if(percent<10) preDigit=3;
        // else if(percent<100) preDigit=4;
        // else preDigit=5;

    }
    free(bytes);
    printf("\n"); 
    int netSize = getFileSize(readFile) - getFileSize(writeFile);
    printf("%d %d\n", getFileSize(readFile), getFileSize(writeFile));
    if(netSize==1 || netSize==-1) return 1;
    else return 0;
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
    return 0; //have to encrypt
}




int main(int argn, char *args[]){

    if(argn<2){  //Check for any file name passed as argument
        // printf("Please right cick on the file and Open with 'encrypter'!!");
        // printf("\nPress Enter to exit!!!");
        //int n; scanf("%d", &n); //input to hold the terminal
        // return 1;
        args[1]="vid.mkv";
        argn=2;
    }


    //key extraction and verification
    int originalKey = extractKey(args[1]); //Check Weather to encrypt of decrypt file and extract key
    int enteredKey;

    if(originalKey==-1){
        printf("Invalid FIle Name!!");
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    else{
        if(originalKey==0){ //take the input for key
            printf("Enter key(1-255) to encrypt this file: "); scanf("%d", &enteredKey);
            if(enteredKey>255 || enteredKey<1){
                printf("Enter the key in Valid range!!!");
                printf("\nPress Enter to exit!!!");
                int n; scanf("%d", &n); //input to hold the terminal
                return 1;
            }
        }
        else{
            printf("Enter key(1-255) to decrypt this file: "); scanf("%d", &enteredKey);
            if(enteredKey>255 || enteredKey<1){
                printf("Enter the key in Valid range!!!");
                printf("\nPress Enter to exit!!!");
                int n; scanf("%d", &n); //input to hold the terminal
                return 1;
            }
            if(originalKey!=enteredKey){
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

    if(originalKey==0) strcat(writeFileName, ".encrypt");
    else {
        char temp[510];
        int len =  strlen(writeFileName)-8;
        strncpy(temp, writeFileName, strlen(writeFileName)-7);
        temp[len]='\0';
        strcpy(writeFileName, temp);
    }

    //Opening files
    printf("Opening File...");
    erase(15);
    FILE *readFile = fopen(readFileName, "rb");
    if (readFile == NULL) {
        printf("Not able to open this file: %s", readFileName);
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    long size = getFileSize(readFile);
    printf("File Size: %d MB  |  %.3f GB\n", size/(1024*1024), (float)size/(1024*1024*1024));

    FILE *writeFile = fopen(writeFileName, "wb");
    if (writeFile == NULL) {
        if(originalKey==0){
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
    if(originalKey==0) {
        a=fwrite(&enteredKey, sizeof(char), 1, writeFile);  //have to encrypt
    }
    else {
        a=fread(&originalKey, sizeof(char), 1, readFile);  //have to decrypt
        size--;
    }

    int N = operate(readFile, writeFile, enteredKey, size);
    fclose(readFile); fclose(writeFile);

    if(N && originalKey) printf("Successfully Decrypted!!!");
    else if(N && !originalKey) printf("Successfully Encrypted!!!");
    else{
        printf("Cannot Encrypt/Decrypt File %d", N);
        remove(writeFileName);
    }

    printf("\nPress Enter to exit!!!\n");
    int n; scanf("%d", &n); //input to hold the terminal
    return 1;
}
