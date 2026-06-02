#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmd[] = "program1.exe 15";

    // Отримання СПРАВЖНЬОГО дескриптора для поточного процесу
    HANDLE hRealProcess;
    DuplicateHandle(
        GetCurrentProcess(), GetCurrentProcess(), 
        GetCurrentProcess(), &hRealProcess, 
        0, FALSE, DUPLICATE_SAME_ACCESS
    );

    // Отримання СПРАВЖНЬОГО дескриптора для поточного основного потоку
    HANDLE hRealThread;
    DuplicateHandle(
        GetCurrentProcess(), GetCurrentThread(), 
        GetCurrentProcess(), &hRealThread, 
        0, FALSE, DUPLICATE_SAME_ACCESS
    );

    std::cout << "--- Програма 2 (Батьківська) ---\n";
    std::cout << "Мій PID: " << GetCurrentProcessId() << " | Мій справжній дескриптор: " << hRealProcess << "\n";
    std::cout << "Мій основний потік TID: " << GetCurrentThreadId() << " | Його справжній дескриптор: " << hRealThread << "\n";
    std::cout << "Запускаю дочірній процес...\n";

    // Створення процесу (CreateProcess повертає справжні дескриптори у структуру pi)
    if (!CreateProcessA(
        NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
    )) {
        std::cerr << "Не вдалося створити процес. Код помилки: " << GetLastError() << "\n";
        CloseHandle(hRealProcess);
        CloseHandle(hRealThread);
        return 1;
    }

    std::cout << "Запущено дочірній процес PID: " << pi.dwProcessId << " | Справжній дескриптор: " << pi.hProcess << "\n";
    std::cout << "Головний потік дочірнього процесу TID: " << pi.dwThreadId << " | Справжній дескриптор: " << pi.hThread << "\n";

    // Очікування завершення дочірнього процесу
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    std::cout << "Дочірній процес успішно завершив роботу.\n";

    // Закриття всіх дескрипторів для уникнення витоків пам'яті
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hRealProcess);
    CloseHandle(hRealThread);

    return 0;
}
