#define SVCNAME TEXT("TestService")

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

void __cdecl _tmain(int argc, TCHAR *argv[]) {
    if (lstrcmpi(argv[1], TEXT("install")) == 0) {
        SvcInstall();
        return;
    }

    if (lstrcmpi(argv[1], TEXT("debug")) == 0) {
        return;
    }

    SERVICE_TABLE_ENTRY DispatchTable[] = {
        {SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain},
        {NULL, NULL}
    };

    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
    }
}
