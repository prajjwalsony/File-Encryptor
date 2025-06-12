#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("original2.pdf", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char text[100000];
    size_t bytesRead = fread(text, sizeof(char), sizeof(text), file);
    if (bytesRead == 0) {
        perror("Error reading file");
        fclose(file);
        return 1;
    }

    fclose(file);

    printf("Read %zu bytes from the file.\n", bytesRead);

    FILE *file2 = fopen("copy.pdf", "wb");
    if (file2 == NULL) {
        perror("Error creating copy file");
        return 1;
    }

    size_t bytesWritten = fwrite(text, sizeof(char), bytesRead, file2);
    if (bytesWritten != bytesRead) {
        perror("Error writing to copy file");
    } else {
        printf("Successfully wrote %zu bytes to the copy file.\n", bytesWritten);
    }

    fclose(file2);

    return 0;
}
