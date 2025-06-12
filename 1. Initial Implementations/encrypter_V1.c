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

int main(int argn, char *args[]) {
    if(argn<2){
        printf("There is Some Error!!");
        int n; scanf("%d", &n);
        return 1;
    }
    // printf("%s\n", args[1]);
    int id = encryptOrDecrypt(args[1]);
    // int n; scanf("%d", &n);

    
    char fileNameOrig[500];
    strcpy(fileNameOrig, args[1]);
    // printf("Enter the name of the file to Encrypt/Decrypt: ");
    // scanf("%s", &fileNameOrig);

    int key;
    printf("Enter key(0-255): "); scanf("%d", &key);

    FILE *file = fopen(fileNameOrig, "rb");
    if (file == NULL) {
        printf("Error opening file %s", fileNameOrig);
        scanf("%s", &fileNameOrig);
        return 1;
    }

    int N = getFileSize(file);
    printf("Space: %d MB\n", N/(1024*1024));
    char *text = (char*)malloc(N);
    if(text==NULL){
        printf("Cannot assign space");
        int cd; scanf("%d", &cd);
        return 1;
    }
    int bytesRead = fread(text, sizeof(char), N, file);
    if (bytesRead == 0) {
        printf("Error reading file %s", fileNameOrig);
        fclose(file);
        scanf("%s", &fileNameOrig);
        return 1;
    }
    fclose(file);

    // printf("Read %zu bytes from the file.\n", bytesRead);

    for(int i=0; i<bytesRead; i++) text[i] = text[i]^key;


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

    FILE *file2 = fopen(fileNameNew, "wb");
    if (file2 == NULL) {
        printf("Error in encrypting/decrypting file");
        scanf("%s", &fileNameOrig);
        return 1;
    }

    size_t bytesWritten = fwrite(text, sizeof(char), bytesRead, file2);
    if (bytesWritten != bytesRead) {
        perror("Cannot Encrypt/Decrypt File");
    } else if(id==0) {
        printf("Successfully Encrypted");
    } else{
        printf("Successfully Decrypted");
    }

    fclose(file2);
    printf("\nPress Enter to Exist");
    scanf("%s", &fileNameOrig);
    return 0;
}
