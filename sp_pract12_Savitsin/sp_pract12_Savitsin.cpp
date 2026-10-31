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

    if (!CreateProcessA(exeName.c_str(), cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
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
    std::cout << "5. Выход\n";
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    bool exit = false;    

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
            std::cout << "3. FindDivisors.exe\n";
            std::cout << "Выберите программу: ";
            int p;
            std::cin >> p;

            if (p == 1)
            {
                StartProcess("C:\\Windows\\System32\\taskmgr.exe", "C:\\Windows\\System32\\taskmgr.exe", "taskmgr");
            }
            else if (p == 2)
            {
                std::cout << "Режимы ping:\n";
                std::cout << "1. ping.exe [IP-address]\n";
                std::cout << "2. ping.exe -n [N] [IP-address]\n";
                std::cout << "3. ping.exe -t [IP-address]\n";
                std::cout << "Выберите режим: ";
                int mode;
                std::cin >> mode;

                std::string ip;
                std::string args = " ";

                if (mode == 1)
                {
                    std::cout << "Введите IP: ";
                    std::cin >> ip;
                    args += ip;
                }
                else if (mode == 2)
                {
                    int n;
                    std::cout << "Введите N: ";
                    std::cin >> n;

                    std::cout << "Введите IP: ";
                    std::cin >> ip;
                    args += "-n " + std::to_string(n) + " " + ip;
                }
                else if (mode == 3)
                {
                    std::cout << "Введите IP: ";
                    std::cin >> ip;
                    args += "-t " + ip;
                }
                else
                {
                    std::cout << "Неверный режим\n";
                    break;
                }

                std::string exePath = "C:\\Windows\\System32\\ping.exe";
                StartProcess(exePath, exePath + args, "ping");
            }
            else if (p == 3)
            {
                int start;
                std::cout << "Введите начало диапозона: ";
                std::cin >> start;

                int end;
                std::cout << "Введите конец диапозона: ";
                std::cin >> end;
                
                char myPath[MAX_PATH];
                GetModuleFileNameA(NULL, myPath, MAX_PATH);
                std::string fullPath(myPath);
                
                std::string folderPath = fullPath.substr(0, fullPath.find_last_of("\\/"));
                
                std::string targetExe = folderPath + "\\FindDivisors.exe";
               
                std::string cmd = "\"" + targetExe + "\" " + std::to_string(start) + " " + std::to_string(end);

                StartProcess(targetExe, cmd, "FindDivisors");
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
            int aliveCount = 0;
            
            for (int i = procCount - 1; i >= 0; i--)
            {
                DWORD exitCode;
                GetExitCodeProcess(processes[i]->hProcess, &exitCode);

                if (exitCode == STILL_ACTIVE)
                {
                    aliveCount++;
                }
            }

            std::cout << "Количество работающих процессов: " << aliveCount << std::endl;
            break;
        }        
        case 5:
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
