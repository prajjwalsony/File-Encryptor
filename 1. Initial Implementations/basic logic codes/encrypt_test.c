#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    FILE *file = fopen("original2.pdf", "r");
    char text[100000];
    // char *ptr = text;
    char ch;
    int i=0;
    // int count = fread(text, sizeof(char), 100000, file);
    size_t bytesRead = fread(text, sizeof(char), sizeof(text), file);
    // ptr=text;
    // printf("%d", count);
    printf("Read %zu bytes from the file.\n", bytesRead);
    fclose(file);
    // FILE *file2 = fopen("copy.pdf", "w");
    // count = fwrite(text, sizeof(char), i, file2);

    
}