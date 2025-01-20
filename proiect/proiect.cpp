#include <windows.h>
#include <iostream>
#include <string>

#define BUFFER_SIZE 256
#define EVENT_NAME L"Global\\FileMappedEvent"
#define FILE_MAPPING_NAME L"Global\\FileMappedMemory"

void writeMessage(HANDLE hMapFile, HANDLE hEvent) {
    std::cout << "Enter your message: ";
    std::string message;
    std::getline(std::cin, message);

    if (message.size() >= BUFFER_SIZE) {
        std::cerr << "Message too long! Maximum " << BUFFER_SIZE - 1 << " characters.\n";
        return;
    }

    LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, BUFFER_SIZE);
    if (pBuf == NULL) {
        std::cerr << "Could not map view of file. Error: " << GetLastError() << "\n";
        return;
    }

    CopyMemory((PVOID)pBuf, message.c_str(), message.size() + 1);
    UnmapViewOfFile(pBuf);

    SetEvent(hEvent);
}

void readMessages(HANDLE hMapFile, HANDLE hEvent) {
    while (true) {
        DWORD dwWaitResult = WaitForSingleObject(hEvent, INFINITE);
        if (dwWaitResult == WAIT_OBJECT_0) {
            LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, BUFFER_SIZE);
            if (pBuf == NULL) {
                std::cerr << "Could not map view of file. Error: " << GetLastError() << "\n";
                return;
            }

            std::cout << "Message received: " << (char*)pBuf << "\n";
            UnmapViewOfFile(pBuf);

            ResetEvent(hEvent);
        }
        else {
            std::cerr << "Error waiting for event. Error: " << GetLastError() << "\n";
            break;
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
        std::cerr << "Could not create file mapping. Error: " << GetLastError() << "\n";
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

    int choice;
    do {
        std::cout << "\nOptions:\n";
        std::cout << "1. Write a message\n";
        std::cout << "2. Listen for messages\n";
        std::cout << "3. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            writeMessage(hMapFile, hEvent);
            break;
        case 2:
            std::cout << "Listening for messages... Press Ctrl+C to exit.\n";
            readMessages(hMapFile, hEvent);
            break;
        case 3:
            std::cout << "Exiting program.\n";
            break;
        default:
            std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 3);

    CloseHandle(hEvent);
    CloseHandle(hMapFile);

    return 0;
}