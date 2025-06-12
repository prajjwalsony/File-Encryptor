#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char fileNameOrig[50];
    printf("Enter the name of the file to Encrypt: ");
    scanf("%s", &fileNameOrig);

    FILE *file = fopen(fileNameOrig, "rb");
    if (file == NULL) {
        printf("Error opening file %s", fileNameOrig);
        return 1;
    }

    char text[5000];
    int bytesRead = fread(text, sizeof(char), sizeof(text), file);
    if (bytesRead == 0) {
        printf("Error reading file %s", fileNameOrig);
        fclose(file);
        return 1;
    }
    fclose(file);

    // printf("Read %zu bytes from the file.\n", bytesRead);

    for(int i=0; i<bytesRead; i++) text[i]-=1;

    char fileNameEncrypt[60];
    strcpy(fileNameEncrypt, fileNameOrig);
    strcat(fileNameEncrypt, ".encrypt");

    FILE *file2 = fopen(fileNameEncrypt, "wb");
    if (file2 == NULL) {
        printf("Error in encrypting file");
        return 1;
    }

    size_t bytesWritten = fwrite(text, sizeof(char), bytesRead, file2);
    if (bytesWritten != bytesRead) {
        perror("Cannot Encrypt File");
    } else {
        printf("Successfully Encrypted File %s \nEncrypted File Name: %s", fileNameOrig, fileNameEncrypt);
    }

    fclose(file2);
    scanf("%s", &fileNameOrig);
    return 0;
}
