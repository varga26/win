#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

// Структура для передачі даних у потік
struct ThreadData {
    int* array;
    int size;
    long long sumOfSquares;
};

// Функція потоку, яка обчислює суму квадратів
DWORD WINAPI CalculateSumOfSquares(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    data->sumOfSquares = 0;
    
    for (int i = 0; i < data->size; ++i) {
        data->sumOfSquares += (long long)(data->array[i]) * data->array[i];
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    int n = 10; 
    if (argc > 1) {
        n = std::atoi(argv[1]);
    }

    int* arr = new int[n];
    std::srand(static_cast<unsigned>(std::time(0)));
    for(int i = 0; i < n; ++i) {
        arr[i] = std::rand() % 10;
    }

    ThreadData tData = {arr, n, 0};
    HANDLE hThread;
    DWORD dwThreadId;

    // Створення додаткового потоку (повертає справжній дескриптор)
    hThread = CreateThread(NULL, 0, CalculateSumOfSquares, &tData, 0, &dwThreadId);

    if (hThread == NULL) {
        std::cerr << "Помилка створення потоку!\n";
        delete[] arr;
        return 1;
    }

    // Отримання СПРАВЖНЬОГО дескриптора для поточного процесу
    HANDLE hRealProcess;
    DuplicateHandle(
        GetCurrentProcess(), GetCurrentProcess(), 
        GetCurrentProcess(), &hRealProcess, 
        0, FALSE, DUPLICATE_SAME_ACCESS
    );

    // Отримання СПРАВЖНЬОГО дескриптора для поточного ГОЛОВНОГО потоку
    HANDLE hRealMainThread;
    DuplicateHandle(
        GetCurrentProcess(), GetCurrentThread(), 
        GetCurrentProcess(), &hRealMainThread, 
        0, FALSE, DUPLICATE_SAME_ACCESS
    );

    std::cout << "\n--- Програма 1 (Дочірня) ---\n";
    std::cout << "Мій PID: " << GetCurrentProcessId() << " | Мій справжній дескриптор процесу: " << hRealProcess << "\n";
    std::cout << "Мій головний потік TID: " << GetCurrentThreadId() << " | Справжній дескриптор головного потоку: " << hRealMainThread << "\n";
    std::cout << "Створено додатковий потік TID: " << dwThreadId << " | Справжній дескриптор додаткового потоку: " << hThread << "\n";

    WaitForSingleObject(hThread, INFINITE);

    std::cout << "Результат: Сума квадратів " << n << " елементів = " << tData.sumOfSquares << "\n";
    std::cout << "----------------------------\n";

    // Звільнення ресурсів
    CloseHandle(hRealProcess);    // Закриваємо здубльований дескриптор процесу
    CloseHandle(hRealMainThread); // Закриваємо здубльований дескриптор головного потоку
    CloseHandle(hThread);         // Закриваємо дескриптор додаткового потоку
    delete[] arr;

    return 0;
}
