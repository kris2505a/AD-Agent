#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <log.h>
#include "agent.h"

SERVICE_STATUS_HANDLE gStatusHandle;
SERVICE_STATUS gStatus {};
std::wstring gServiceName = L"ADAgent";


DWORD WINAPI serviceControl(DWORD control, DWORD eventType, LPVOID eventData, LPVOID context) {
    switch (control) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN: {
        gStatus.dwCurrentState = SERVICE_STOP_PENDING;
        gStatus.dwControlsAccepted = 0;

        SetServiceStatus(gStatusHandle, &gStatus);

        Agent::stop();
        
        gStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(gStatusHandle, &gStatus);

        return NO_ERROR;
    }
    }
    return NO_ERROR;
}

void WINAPI serviceMain(DWORD argc, LPWSTR* argv) {
    gStatusHandle = RegisterServiceCtrlHandlerExW(gServiceName.data(), serviceControl, nullptr);

    if (!gStatusHandle) {
        Log::error("Failed to create service handle!");
        return;
    }
    
    gStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gStatus.dwCurrentState = SERVICE_START_PENDING;
    gStatus.dwControlsAccepted = 0;
    gStatus.dwWin32ExitCode = NOERROR;
    gStatus.dwServiceSpecificExitCode = 0;
    gStatus.dwCheckPoint = 0;
    gStatus.dwWaitHint = 0;

    SetServiceStatus(gStatusHandle, &gStatus);

    Agent::start();

    gStatus.dwCurrentState = SERVICE_RUNNING;
    gStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

    SetServiceStatus(gStatusHandle, &gStatus);
}


int main() {
    
    SERVICE_TABLE_ENTRYW serviceTable[] = {
        { gServiceName.data(), serviceMain },
        { nullptr, nullptr }
    };

    StartServiceCtrlDispatcherW(serviceTable);

    return 0;
}


