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

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789-_";

//------------------------------------------------------------------------------

std::string base64_encode(unsigned char const*, unsigned int);

//------------------------------------------------------------------------------

string file_to_string(string);

void wait(void);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
