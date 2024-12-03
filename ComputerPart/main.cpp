#include <Windows.h>
#include <iostream>
#include <thread>
#include <conio.h>

//#define DEBUG

enum CPU_LOAD {
    MIN = 'L', // 0-40%
    MID = 'M', // 40-80%
    MAX = 'H', // 80-100%
};

double FileTimeToInt64(const FILETIME & ft) {
    return (double)(((ft.dwHighDateTime << 32)) | ft.dwLowDateTime);
}

CPU_LOAD GetCPULoad() {
    FILETIME idletime, kerneltime, usertime;
    FILETIME n_idletime, n_kerneltime, n_usertime;
    GetSystemTimes(&idletime, &kerneltime, &usertime);
    Sleep(500);
    GetSystemTimes(&n_idletime, &n_kerneltime, &n_usertime);

    double result_idletime = FileTimeToInt64(n_idletime) - FileTimeToInt64(idletime);
    double result_kerneltime = FileTimeToInt64(n_kerneltime) - FileTimeToInt64(kerneltime);
    double result_usertime = FileTimeToInt64(n_usertime) - FileTimeToInt64(usertime);   
    double cpu_usage = 100*(1.0 - result_idletime/(result_kerneltime + result_usertime));

#ifdef DEBUG
    std::cout << "GetCPULoad cpu usage: " << cpu_usage << std::endl;
#endif

    if (cpu_usage < 40) return CPU_LOAD::MIN;
    if (cpu_usage >= 40 && cpu_usage < 80) return CPU_LOAD::MID;
    if (cpu_usage >= 80) return CPU_LOAD::MAX;
}

void SendCPUInfoCom(CPU_LOAD cpu_usage) {
    HANDLE serial = CreateFile(TEXT("COM8"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
    // instead of com8, specify the com port to which your arduino is connected
    if (serial == INVALID_HANDLE_VALUE) {
        
#ifdef DEBUG
        std::cerr << "serial == INVALID_HANDLE_VALUE" << std::endl;
#endif        
        throw "serial == INVALID HANDLE VALUE";
    }
    DCB arduino;
    arduino.DCBlength = sizeof(DCB);
    arduino.BaudRate = CBR_9600;
    arduino.Parity = MARKPARITY;
    arduino.ByteSize = 8;
    arduino.StopBits = ONE5STOPBITS;
    
    if(!SetCommState(serial, &arduino)) {

#ifdef DEBUG
        std::cerr << "SetCommState error" << std::endl;
#endif
        throw "SetCommState error";
    }

    PurgeComm(serial, PURGE_TXCLEAR | PURGE_TXCLEAR);

    DWORD bytes_write = 0;
    
#ifdef DEBUG
    std::cerr << "SendCPUInfoCom cpu usage: " << (char)cpu_usage << std::endl;
    std::cerr << "SendCPUInfoCom sizeof CPU_LOAD: " << sizeof(CPU_LOAD) << std::endl;
#endif

    WINBOOL result = WriteFile(serial, (char*)&cpu_usage, sizeof(char), &bytes_write, NULL);
    if(!result) {

#ifdef DEBUG
        std::cerr << "!WriteFile" << std::endl;
#endif
        throw "!WriteFile(serial...)";
    }
    CloseHandle(serial);
    serial = INVALID_HANDLE_VALUE;
}

int main() {
    std::thread com_thread([]() {
        while(true) {
            SendCPUInfoCom(GetCPULoad());
            Sleep(1000);
        }
    });
    std::cout << "To exit, press e: " << std::endl;
    while( _getch() != 'e' );
    com_thread.detach();
    return 0;
}
