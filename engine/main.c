/*
UPDATE HISTORY:
16/12/2025 - first made

-------------------------------------------------------------------------------

DEPENDENCIES:
all
main entry point file

-------------------------------------------------------------------------------

FILE DESCRIPTION:
provide cli based interface for uses
*/

#include "AES_Encrypter.h"

char *fileLocation = NULL;

void erase(int n){
    while(n--) printf("\b");
}

DWORD WINAPI printInfo(LPVOID arg){
    // processedFileSize;
    HANDLE hFile = CreateFile(fileLocation, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }
    LARGE_INTEGER size;
    GetFileSizeEx(hFile, &size);
    CloseHandle(hFile);
    double TOTAL_FILE_SIZE = (double)size.QuadPart;
    while(IS_COMPLETED==0){
        Sleep(1);
        printf("%.2f %%", (processedFileSize*100)/(TOTAL_FILE_SIZE));
        erase(50);
    }
}

int main(int argn, char *args[]){

    if(argn<2){  //Check for any file name passed as argument
        char *name = (char*)malloc(50);
        printf("Enter the file name: ");
        scanf("%s",name);
        args[1]=name;
    }

    fileLocation = args[1];
    char pwd[5];
    printf("Enter password: ");
    scanf("%s", pwd);

    HANDLE threads[1];
    threads[0] = CreateThread(
        NULL,
        0,
        printInfo,
        NULL,
        0,
        NULL
    );
    int status = AES(args[1], pwd);
    WaitForMultipleObjects(1, threads, TRUE, INFINITE);
    CloseHandle(threads[0]);
    printf("Speed: %d MBPS\n", SPEED/(1024*1024));
    printf("STATUS: %d", status);
    scanf("%d", &status);
}