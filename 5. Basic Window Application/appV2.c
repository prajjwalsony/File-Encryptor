// Secure.c
// Windows GUI for AES_Encrypter in pure C

#include <windows.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <stdio.h>
#include <string.h>
#include "AES_Encrypter.h"

#define ID_FILE_BTN 1
#define ID_KEY_BOX  2
#define ID_RUN_BTN  3
#define ID_LABEL    5
#define IDI_ICON1   101  // Ensure this matches your .rc definition

char filePath[MAX_PATH] = "";

void ShowError(HWND hwnd, int code) {
    const char* messages[] = {
        "Key not match during decryption", "Invalid file name or cannot open file", "Password format incorrect",
        "Cannot open input file", "Cannot create output file", "Cannot write output file",
        "Cannot read input file", "Cannot encrypt or decrypt", "Memory allocation error",
        "Key generation error", "Input/output length mismatch"
    };
    int index = code == 0 ? 0 : -code;
    if (index >= 0 && index <= 10) {
        MessageBox(hwnd, messages[index], "Error", MB_ICONERROR);
    } else {
        MessageBox(hwnd, "Unknown error", "Error", MB_ICONERROR);
    }
}

int EndsWithEncrypt(const char* path) {
    const char* ext = PathFindExtensionA(path);
    return ext && _stricmp(ext, ".encrypt") == 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND fileBtn, keyEdit, runBtn, label;

    switch (msg) {
        case WM_CREATE:
            CreateWindow("STATIC", "File:", WS_VISIBLE | WS_CHILD, 10, 20, 30, 20, hwnd, NULL, NULL, NULL);
            fileBtn = CreateWindow("EDIT", filePath, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 50, 20, 300, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "...", WS_VISIBLE | WS_CHILD, 360, 20, 30, 20, hwnd, (HMENU)ID_FILE_BTN, NULL, NULL);

            CreateWindow("STATIC", "Key:", WS_VISIBLE | WS_CHILD, 10, 60, 30, 20, hwnd, NULL, NULL, NULL);
            keyEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 50, 60, 100, 20, hwnd, (HMENU)ID_KEY_BOX, NULL, NULL);

            runBtn = CreateWindow("BUTTON", "Encrypt", WS_VISIBLE | WS_CHILD, 170, 60, 100, 25, hwnd, (HMENU)ID_RUN_BTN, NULL, NULL);

            label = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD, 10, 100, 380, 40, hwnd, (HMENU)ID_LABEL, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_FILE_BTN) {
                OPENFILENAME ofn = {0};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = filePath;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter = "All Files\0*.*\0";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if (GetOpenFileName(&ofn)) {
                    SetWindowText(fileBtn, filePath);
                    SetWindowText(runBtn, EndsWithEncrypt(filePath) ? "Decrypt" : "Encrypt");
                }
            } else if (LOWORD(wParam) == ID_RUN_BTN) {
                char key[5];
                GetWindowText(keyEdit, key, 5);
                GetWindowText(fileBtn, filePath, MAX_PATH);

                size_t len = strlen(filePath);
                if (filePath[0] == '"' && filePath[len - 1] == '"') {
                    memmove(filePath, filePath + 1, len - 2);
                    filePath[len - 2] = '\0';
                }

                if (strlen(key) != 4) {
                    ShowError(hwnd, -2);
                    return 0;
                }

                HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile == INVALID_HANDLE_VALUE) {
                    ShowError(hwnd, -1);
                    return 0;
                }
                LARGE_INTEGER size;
                GetFileSizeEx(hFile, &size);
                CloseHandle(hFile);

                double expected = (double)size.QuadPart / (1024.0 * 1024.0) / 23.0;
                char estmsg[100];
                sprintf(estmsg, "Estimated time: %.2f sec (at 23 MB/s)", expected);
                SetWindowText(label, estmsg);

                SetWindowText(runBtn, "Processing...");
                UpdateWindow(hwnd);

                double result = AES(filePath, key);

                if (result > 0) {
                    double seconds = result;
                    double speed = (double)size.QuadPart / (1024.0 * 1024.0) / seconds;
                    char msg[100];
                    sprintf(msg, "Done in %.2f sec (%.2f MB/s)", seconds, speed);
                    SetWindowText(label, msg);
                } else {
                    ShowError(hwnd, result);
                }
                SetWindowText(runBtn, EndsWithEncrypt(filePath) ? "Decrypt" : "Encrypt");
            }
            break;

        case WM_DROPFILES:
            DragQueryFile((HDROP)wParam, 0, filePath, MAX_PATH);
            SetWindowText(fileBtn, filePath);
            SetWindowText(runBtn, EndsWithEncrypt(filePath) ? "Decrypt" : "Encrypt");
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (lpCmdLine && strlen(lpCmdLine) > 0) {
        strncpy(filePath, lpCmdLine, MAX_PATH - 1);
        filePath[MAX_PATH - 1] = '\0';
        size_t len = strlen(filePath);
        if (filePath[0] == '"' && filePath[len - 1] == '"') {
            memmove(filePath, filePath + 1, len - 2);
            filePath[len - 2] = '\0';
        }
    }

    WNDCLASSEX wc = {0};  // Changed to WNDCLASSEX
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SecureWin";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClassEx(&wc);  // Use RegisterClassEx instead of RegisterClass

    HWND hwnd = CreateWindow("SecureWin", "Secure File Encryptor", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 200, NULL, NULL, hInstance, NULL);

    DragAcceptFiles(hwnd, TRUE);
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
