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

int main() {
    printf("Want to encrypt(0) or decrypt(1): ");
    int id; scanf("%d", &id);

    char fileNameOrig[50];
    printf("Enter the name of the file to Encrypt/Decrypt: ");
    scanf("%s", &fileNameOrig);

    int key;
    printf("Enter key(0-255): "); scanf("%d", &key);

    FILE *file = fopen(fileNameOrig, "rb");
    if (file == NULL) {
        printf("Error opening file %s", fileNameOrig);
        scanf("%s", &fileNameOrig);
        return 1;
    }

    int N = getFileSize(file);
    printf("Space: %d KB\n", N/1024);
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


    char fileNameNew[60];
    strcpy(fileNameNew, fileNameOrig);
    if(id==0) strcat(fileNameNew, ".encrypt");
    else {
        char temp[60];
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
    } else {
        printf("Successfully Encrypted/Decrypted File from %s \n To File Name: %s", fileNameOrig, fileNameNew);
    }

    fclose(file2);
    scanf("%s", &fileNameOrig);
    return 0;
}
