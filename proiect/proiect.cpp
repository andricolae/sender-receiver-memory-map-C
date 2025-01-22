#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <limits>

#define BUFFER_SIZE 256
#define EVENT_NAME L"Global\\FileMappedEvent"
#define FILE_MAPPING_NAME L"Global\\FileMappedMemory"

void writeMessage(HANDLE hMapFile, HANDLE hEvent) {
    std::cout << "Enter message: ";
    std::string message;
    std::getline(std::cin, message);

    if (message.size() >= BUFFER_SIZE) {
        std::cerr << "Message too long! Max. " << BUFFER_SIZE - 1 << " chars.\n";
        return;
    }

    LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, BUFFER_SIZE);
    if (pBuf == NULL) {
        std::cerr << "Could not map file. Error: " << GetLastError() << "\n";
        return;
    }

    CopyMemory((PVOID)pBuf, message.c_str(), message.size() + 1);
    UnmapViewOfFile(pBuf);

    SetEvent(hEvent);
}

void readMessages(HANDLE hMapFile, HANDLE hEvent) {
    bool isRunning = true;
    while (isRunning) {
        if (_kbhit()) {
            std::string input;
            std::getline(std::cin, input);
            if (input == "switch") {
                std::cout << "Switching role...\n";
                break;
            }
        }

        DWORD dwWaitResult = WaitForSingleObject(hEvent, 100);
        if (dwWaitResult == WAIT_OBJECT_0) {
            LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, BUFFER_SIZE);
            if (pBuf == NULL) {
                std::cerr << "Could not map file. Error: " << GetLastError() << "\n";
                break;
            }

            std::cout << "Message received: " << (char*)pBuf << "\n";
            UnmapViewOfFile(pBuf);

            ResetEvent(hEvent);
        }
    }
}

int main() {
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,  
        NULL,                  
        PAGE_READWRITE,        
        0,                     
        BUFFER_SIZE,           
        FILE_MAPPING_NAME);    

    if (hMapFile == NULL) {
        std::cerr << "Could not create file map. Error: " << GetLastError() << "\n";
        return 1;
    }

    HANDLE hEvent = CreateEvent(
        NULL,                  
        TRUE,                  
        FALSE,                 
        EVENT_NAME);           

    if (hEvent == NULL) {
        std::cerr << "Could not create event. Error: " << GetLastError() << "\n";
        CloseHandle(hMapFile);
        return 1;
    }

    bool isRunning = true;
    int role = 0;

    while (isRunning) {
        if (role == 0) {
            std::cout << "\nChoose role:\n";
            std::cout << "1. Sender\n";
            std::cout << "2. Receiver\n";
            std::cout << "3. Exit\n";
            std::cout << "Choose an option: ... ";
            std::cin >> role;
            std::cin.ignore(); 
            if (role == 3) {
                isRunning = false;
                break;
            }
        }

        if (role == 1) {
            std::cout << "\n----- Sender -----\n";

            bool isRunning = true;
            while (isRunning) {
                std::cout << "Type message (type 'switch' to change role): ";

                std::string message;
                std::getline(std::cin, message);

                if (message == "switch") {
                    std::cout << "Switching role...\n";
                    role = 0; 
                    isRunning = false;
                    break;
                }

                LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, BUFFER_SIZE);
                if (pBuf == NULL) {
                    std::cerr << "Could not map file. Error: " << GetLastError() << "\n";
                    break;
                }
                CopyMemory((PVOID)pBuf, message.c_str(), message.size() + 1); 
                UnmapViewOfFile(pBuf);

                if (!SetEvent(hEvent)) {
                    std::cerr << "Failed to signal event. Error: " << GetLastError() << "\n";
                }

                std::cout << "Message sent: " << message << "\n";
            }
        }

        else if (role == 2) { 
            std::cout << "\n----- Receiver -----\n";
            std::cout << "Listening for messages... (Type 'switch' to change role)\n";

            bool isRunning = true;
            while (isRunning) {
                if (_kbhit()) {
                    std::string input;
                    std::getline(std::cin, input);
                    if (input == "switch") {
                        std::cout << "Switching role...\n";
                        role = 0;
                        isRunning = false;
                        break;
                    }
                }

                DWORD dwWaitResult = WaitForSingleObject(hEvent, 100); 
                if (dwWaitResult == WAIT_OBJECT_0) {
                    LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, BUFFER_SIZE);
                    if (pBuf == NULL) {
                        std::cerr << "Could not map file. Error: " << GetLastError() << "\n";
                        isRunning = false;
                        break;
                    }
                    std::cout << "Message received: " << (char*)pBuf << "\n";
                    UnmapViewOfFile(pBuf);

                    ResetEvent(hEvent);
                }
            }
        }

        else {
            std::cout << "Invalid role\n";
            role = 0;
        }
    }

    CloseHandle(hEvent);
    CloseHandle(hMapFile);

    std::cout << "Program exited\n";
    return 0;
}
