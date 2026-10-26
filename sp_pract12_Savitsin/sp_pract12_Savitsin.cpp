#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <string>

struct ProcessInfo
{
    DWORD pid;
    HANDLE hProcess;
    std::string name;
};

ProcessInfo* processes[50];
int procCount = 0;

void FindDivisors(int start, int end)
{
    int maxDivs = 0;
    int bestNum = start;

    std::cout << "Поиск числа с максимальным количеством делителей:\n";

    for (int i = start; i <= end; i++)
    {
        int divs = 0;
        for (int j = 1; j <= i; j++)
        {
            if (i % j == 0)
            {
                divs++;
            }
        }

        std::cout << "Число: " << i << ". Делителей: " << divs << std::endl;

        if (divs > maxDivs)
        {
            maxDivs = divs;
            bestNum = i;
        }
    }

    std::cout << "Число с наибольшим количеством делителей: " << bestNum << ". Делителей: " << maxDivs << std::endl;

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.get();
    std::cin.get(); 
}

void StartProcess(std::string exeName, std::string cmdLine, std::string shortName)
{
    if (procCount >= 50) return;
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char* cmd = new char[cmdLine.length() + 1];
    strcpy_s(cmd, cmdLine.length() + 1, cmdLine.c_str());

    if (!CreateProcessA(exeName.c_str(), cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        std::cout << "Ошибка запуска: " << GetLastError() << std::endl;
    }
    else
    {
        std::cout << "Процесс запущен. PID: " << pi.dwProcessId << std::endl;
        processes[procCount] = new ProcessInfo;
        processes[procCount]->pid = pi.dwProcessId;
        processes[procCount]->hProcess = pi.hProcess;
        processes[procCount]->name = shortName;
        procCount++;
        CloseHandle(pi.hThread);
    }

    delete[] cmd;
}

void PrintMenu()
{
    std::cout << "\nМеню:\n";
    std::cout << "1. Создание процессов\n";
    std::cout << "2. Принудительное завершение выбранных процессов\n";
    std::cout << "3. Отображение списка созданных процессов\n";
    std::cout << "4. Отображение количества работающих процессов\n";
    std::cout << "5. Передача аргументов командной строки при запуске процессов\n";
    std::cout << "6. Выход\n";
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    bool exit = false;

    if (argc == 3)
    {
        int start = atoi(argv[1]);
        int end = atoi(argv[2]);
        FindDivisors(start, end);
        return 0;
    }

    while (!exit)
    {
        PrintMenu();

        std::cout << "Выберите пункт меню: ";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            std::cout << "Программы:\n";
            std::cout << "1. taskmgr.exe\n";
            std::cout << "2. ping.exe\n";
            std::cout << "3. sp_pract12_Savitsin.exe\n";
            std::cout << "Выберите программу: ";
            int p;
            std::cin >> p;

            if (p == 1)
            {
                StartProcess("C:\\Windows\\System32\\taskmgr.exe", "C:\\Windows\\System32\\taskmgr.exe", "taskmgr");
            }
            else if (p == 2)
            {
                StartProcess("C:\\Windows\\System32\\ping.exe", "C:\\Windows\\System32\\ping.exe 127.0.0.1", "ping");
            }
            else if (p == 3)
            {
                int start;
                std::cout << "Введите начало диапозона: ";
                std::cin >> start;

                int end;
                std::cout << "Введите конец диапозона: ";
                std::cin >> end;

                std::string myPath = argv[0]; 
                std::string cmd = "\"" + std::string(myPath) + "\" " + std::to_string(start) + " " + std::to_string(end);
                StartProcess(myPath, cmd, "sp_pract12_Savitsin");
                
                WaitForSingleObject(processes[procCount - 1]->hProcess, INFINITE);
            }
            break;
        }
        case 2:
        {
            if (procCount == 0)
            {
                std::cout << "Нет запущенных процессов\n";
                break;
            }

            std::cout << "Список процессов:\n";
            for (int i = 0; i < procCount; i++)
            {
                std::cout << i + 1 << ": " << processes[i]->name << ". PID: " << processes[i]->pid << std::endl;
            }

            std::cout << "Введите номер процесса для завершения: ";
            int num;
            std::cin >> num;

            if (num > 0 && num <= procCount)
            {
                int index = num - 1;

                if (TerminateProcess(processes[index]->hProcess, 0))
                {
                    std::cout << "Процесс завершен\n";
                }
                else
                {
                    std::cout << "Не удалось завершить процесс\n";
                }

                CloseHandle(processes[index]->hProcess);
                delete processes[index];

                for (int i = index; i < procCount - 1; i++)
                {
                    processes[i] = processes[i + 1];
                }
                procCount--;
            }
            break;
        }
        case 3:
        {
            if (procCount == 0)
            {
                std::cout << "Список пуст\n";
                break;
            }

            std::cout << "Список созданных процессов:\n";
            for (int i = 0; i < procCount; i++)
            {
                DWORD exitCode;
                GetExitCodeProcess(processes[i]->hProcess, &exitCode);

                std::string state = (exitCode == STILL_ACTIVE) ? "Работает" : "Завершен";

                std::cout << i + 1 << ". Название: " << processes[i]->name <<
                    ". ID: " << processes[i]->pid <<
                    ". Состояние: " << state << std::endl;
            }
            break;
        }
        case 4:
        {
            std::cout << "Количество работающих процессов: " << procCount << std::endl;
            break;
        }
        case 5:
        {
            std::cout << "Режимы ping:\n";
            std::cout << "1. ping.exe [IP-address]\n";
            std::cout << "2. ping.exe -n [N] [IP-address]\n";
            std::cout << "3. ping.exe -t [IP-address]\n";
            std::cout << "Выберите режим: ";
            int mode;
            std::cin >> mode;

            std::string ip;
            std::string cmd = "ping.exe ";

            if (mode == 1)
            {
                std::cout << "Введите IP: ";
                std::cin >> ip;
                cmd += ip;
            }
            else if (mode == 2)
            {
                int n;
                std::cout << "Введите N: ";
                std::cin >> n;

                std::cout << "Введите IP: ";
                std::cin >> ip;
                cmd += "-n " + std::to_string(n) + " " + ip;
            }
            else if (mode == 3)
            {
                std::cout << "Введите IP: ";
                std::cin >> ip;
                cmd += "-t " + ip;
            }
            else
            {
                std::cout << "Неверный режим\n";
                break;
            }

            StartProcess("C:\\Windows\\System32\\ping.exe", "C:\\Windows\\System32\\ping.exe " + cmd.substr(9), "ping");
            break;
        }
        case 6:
        {
            exit = true;

            for (int i = 0; i < procCount; i++)
            {
                CloseHandle(processes[i]->hProcess);
                delete processes[i];
            }
            break;
        }
        default:
        {
            std::cout << "Такого пункта меню нет\n";
            break;
        }
        }
    }
}
