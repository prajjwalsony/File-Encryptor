#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long getFileSize(FILE *file) {
    // Move file pointer to the end of the file
    fseek(file, 0, SEEK_END);
    
    // Get the current position of the file pointer, which is the size of the file
    long fileSize = ftell(file);
    
    // Move file pointer back to the beginning of the file (optional if you need to read it)
    fseek(file, 0, SEEK_SET);
    
    return fileSize;
}

int encryptOrDecrypt(char *fileName){
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
    if(strcmp("encrypt", extension)==0) return 1; //have to decrypt
    return 0; //have to encrypt
}

void erase(int n){ //erase the previous printed text in terminal
    for(int i=0; i<n; i++) printf("\b");
}

int main(int argn, char *args[]) {
    // args[1]="D:\\fileEncryption\\original.pdf";
    // argn=2;
    if(argn<2){  //Check for any file name passed as argument
        printf("Please right cick on the file and Open with 'encrypter'!!");
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    int id = encryptOrDecrypt(args[1]); //Check Weather to encrypt of decrypt file
    
    if(id==-1){
        printf("Invalid FIle Name!!");
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    
    char fileNameOrig[500];
    strcpy(fileNameOrig, args[1]); //copy the file location to fileNameOrig

    int key;
    if(id==0){ //take the input for key
        printf("Enter key(1-255) to encrypt this file: "); scanf("%d", &key);
    }
    else{
        printf("Enter key(1-255) to decrypt this file: "); scanf("%d", &key);
    }

    if(key>255 || key<1){
        printf("Enter a valid Key Value!!!");
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    
    printf("Opening File...");
    FILE *file = fopen(fileNameOrig, "rb");
    erase(15);
    if (file == NULL) {
        printf("Not able to open this file: %s", fileNameOrig);
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }

    //File size calculation and assigning memory
    long N = getFileSize(file);
    printf("File Size: %d MB  |  %.3f GB\n", N/(1024*1024), (float)N/(1024*1024*1024));
    char *text = (char*)malloc(N);
    if(text==NULL){
        printf("This file is too large! Please Try Again");
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }

    //enter to exist

    //Read the file data
    printf("Reading File...");
    int bytesRead = fread(text, sizeof(char), N, file);
    erase(15);
    if (bytesRead == 0) {
        printf("Cannot reading file: %s", fileNameOrig);
        printf("Try Again!!!");
        fclose(file);
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }
    fclose(file);

    //Encryption/Decryption
    if(id==0) printf("Encrypting...");
    else printf("Decrypting...");
    for(int i=0; i<bytesRead; i++) text[i] = text[i]^key;
    erase(13);

    //Generate the new File Name
    char fileNameNew[510];
    fileNameNew[509]='\0';
    strcpy(fileNameNew, fileNameOrig);
    if(id==0) strcat(fileNameNew, ".encrypt");
    else {
        char temp[510];
        int len =  strlen(fileNameNew)-8;
        strncpy(temp, fileNameNew, strlen(fileNameNew)-7);
        temp[len]='\0';
        strcpy(fileNameNew, temp);
    }

    //Write the encrypted/decrypted data to new file
    FILE *file2 = fopen(fileNameNew, "wb");
    if (file2 == NULL) {
        if(id==0){
            printf("There is Error during creating a encrypted file");
        }
        else{
            printf("There is Error during creating a decrypted file");
        }
        printf("\nPress Enter to exit!!!");
        int n; scanf("%d", &n); //input to hold the terminal
        return 1;
    }


    size_t bytesWritten = fwrite(text, sizeof(char), bytesRead, file2);
    if (bytesWritten != bytesRead) {
        perror("There is some error!!! Cannot Encrypt/Decrypt File");
    } else if(id==0) {
        printf("Successfully Encrypted");
    } else{
        printf("Successfully Decrypted");
    }

    free(text);
    fclose(file2);
    printf("\nPress Enter to exit!!!");
    int n; scanf("%d", &n); //input to hold the terminal
    return 0;
}
