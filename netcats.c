#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
///usr/bin/i686-w64-mingw32-gcc terminal.c -o terminal.exe -mwindows
#include <windows.h>
#include <string.h>
#include <stdio.h>
#define ID_SCROLL 1
char bbuffer[4096]="curl ";
char *c1=bbuffer;
// Buffer para armazenar o texto
char *textBuffer;
char *textBuffer2;
int textLen = 0;
int scrollPos = 0;
int MAX_TEXT=4096*16;
char ccc=0;
void ExecuteCommand(char* cmd) {
    char buffer[1024];
    char cmds[4096]; 
    FILE* pipes;
    FILE* f2;
    c1=bbuffer+strlen(bbuffer); 
    textLen=strlen(textBuffer);
    if (textLen > MAX_TEXT + 10) {
            MAX_TEXT=MAX_TEXT+10;
            textBuffer=(char *)realloc(textBuffer,MAX_TEXT);
            textBuffer2=(char *)realloc(textBuffer2,MAX_TEXT);
    }
    strcat(textBuffer, "\r\n");
    // abrir processo cmd
    
   
    strcpy(cmds,bbuffer);
    strcat(cmds," > log.txt\n");
    freopen("log.txt", "w", stdout);
        
    system(cmds);
    fclose (stdout);
    if ( (pipes =fopen("log.txt", "r"))==NULL) {
            textLen=strlen(textBuffer);
            if (textLen > MAX_TEXT + 100) {
                MAX_TEXT=MAX_TEXT+100;
                textBuffer=(char *)realloc(textBuffer,MAX_TEXT);
                textBuffer2=(char *)realloc(textBuffer2,MAX_TEXT);
            }

        strcat(textBuffer, "Error \r\n");
        return;
    }

    while (fgets(buffer, sizeof(buffer), pipes)) {
        textLen=strlen(textBuffer);
        if (textLen > MAX_TEXT + 100+strlen(buffer)) {
                MAX_TEXT=MAX_TEXT+10+strlen(buffer);
                textBuffer=(char *)realloc(textBuffer,MAX_TEXT);
                textBuffer2=(char *)realloc(textBuffer2,MAX_TEXT);
         }

        strcat(textBuffer, buffer);
        strcat(textBuffer, "\r");
    }
    fclose(pipes);
    textLen=strlen(textBuffer);
    if (textLen > MAX_TEXT + 10) {
            MAX_TEXT=MAX_TEXT+10;
            textBuffer=(char *)realloc(textBuffer,MAX_TEXT);
            textBuffer2=(char *)realloc(textBuffer2,MAX_TEXT);
    }


    strcat(textBuffer, "\r\n");
    
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HFONT hFont;

    switch (msg) {
    case WM_CREATE: {
        // Criar fonte de 16px
        hFont = CreateFontA(
            -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            FF_DONTCARE, "Consolas");

        // Criar scroll bar vertical
        CreateWindowExA(0, "SCROLLBAR", NULL,
            WS_CHILD | WS_VISIBLE | SBS_VERT,
            750, 0, 20, 600, hwnd, (HMENU)ID_SCROLL,
            GetModuleHandle(NULL), NULL);

        SCROLLINFO si = { sizeof(si) };
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = 100;
        si.nPage = 20;
        SetScrollInfo(GetDlgItem(hwnd, ID_SCROLL), SB_CTL, &si, TRUE);
    } break;

    case WM_VSCROLL: {
        HWND hScroll = (HWND)lParam;
        SCROLLINFO si = { sizeof(si) };
        si.fMask = SIF_ALL;
        GetScrollInfo(hScroll, SB_CTL, &si);

        int pos = si.nPos;
        switch (LOWORD(wParam)) {
        case SB_LINEUP:   pos -= 1; break;
        case SB_LINEDOWN: pos += 1; break;
        case SB_PAGEUP:   pos -= si.nPage; break;
        case SB_PAGEDOWN: pos += si.nPage; break;
        case SB_THUMBTRACK: pos = si.nTrackPos; break;
        }
        if (pos < si.nMin) pos = si.nMin;
        if (pos > si.nMax - (int)si.nPage + 1) pos = si.nMax - si.nPage + 1;

        si.fMask = SIF_POS;
        si.nPos = pos;
        SetScrollInfo(hScroll, SB_CTL, &si, TRUE);

        scrollPos = pos;
        InvalidateRect(hwnd, NULL, TRUE);
    } break;
    case WM_CHAR: {
        ccc=(char)wParam;
        textLen=strlen(bbuffer);
        if (textLen<4001){
            if(ccc!=13)bbuffer[textLen] = ccc;
            bbuffer[textLen+1] = '\0';
            strcpy(textBuffer,bbuffer);
        }

        textLen=strlen(textBuffer);
        if (textLen > MAX_TEXT + 10) {
            MAX_TEXT=MAX_TEXT+4096;
            textBuffer=(char *)realloc(textBuffer,MAX_TEXT);
            textBuffer2=(char *)realloc(textBuffer2,MAX_TEXT);
        }
        
        if (ccc==13){
            
            textBuffer[textLen] = '\0';
            ExecuteCommand(bbuffer);
            strcpy(bbuffer,"curl ");
        }
        InvalidateRect(hwnd, NULL, TRUE);
        
    } break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 0));
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);

        SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        strcpy(textBuffer2,textBuffer);
        int y = 10;
        char *line = strtok(textBuffer2, "\r");
        while (line!=NULL) {
            TextOutA(hdc, 10, 10 - scrollPos * 20+y , line, strlen(line));
            
            y += 20; // próxima linha (altura da fonte)
            line = strtok(NULL, "\r");
        }

        

        EndPaint(hwnd, &ps);
    } break;

    case WM_DESTROY:
        free(textBuffer);
        free(textBuffer2);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "YellowWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);
    textBuffer=(char *)malloc(MAX_TEXT);
    strcpy(textBuffer,"\n");
    
    textBuffer2=(char *)malloc(MAX_TEXT);
    HWND hwnd = CreateWindowA("YellowWindow", "Janela Amarela",
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                              NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
