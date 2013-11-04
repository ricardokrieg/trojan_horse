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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class ServiceManager {
    public:
        ServiceManager(string);
        ~ServiceManager(void);

        void main(bool);
        int install(void);
        void copy(void);
        void start_service_ctrl_dispatcher(void);
        void init(DWORD, LPTSTR*);
        bool launch_process(string);

        void report_status(DWORD, DWORD, DWORD);
        void report_event(string);

        void log(string, bool show_last_error=true);

        bool running;
        string name;
        string path;
        string unique_id;
        SERVICE_STATUS gSvcStatus;
        SERVICE_STATUS_HANDLE gSvcStatusHandle;
        HANDLE ghSvcStopEvent;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
