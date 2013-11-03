#include "helper.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT  num = 0, size = 0;

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if(size == 0) return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL) return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j) {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

//------------------------------------------------------------------------------

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret; int i = 0; int j = 0; unsigned char char_array_3[3]; unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];
    }

    return ret;
}

//------------------------------------------------------------------------------

void wait(void) {
    usleep(1000 * 500);
}

//------------------------------------------------------------------------------

void set_machine_id(void) {
    HKEY hKey;
    DWORD dwDisp = 0;
    LPDWORD lpdwDisp = &dwDisp;

    srand(time(0));
    string random_id;
    for (int j=0; j < 64; j++) {
        random_id += base64_chars[rand() % base64_chars.length()];
    }

    cout << "generated UniqueID: " << random_id << endl;

    LONG result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, registry_key.c_str(), 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, lpdwDisp);

    if (result == ERROR_SUCCESS) {
        cout << "Registry Key created" << endl;
        RegSetValueEx(hKey, NULL, 0, REG_SZ, (LPBYTE)random_id.c_str(), random_id.length());
    } else {
        cout << "Error creating Registry Key: " << result << endl;
    }
}

string get_machine_id(void) {
    HKEY hKey;

    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, registry_key.c_str(), 0, KEY_ALL_ACCESS, &hKey);

    if (result == ERROR_SUCCESS) {
        char value[255];
        DWORD dwDisp = 0;
        DWORD data_size = 255;
        memset(value, 0, 255);

        LONG query_result = RegQueryValueEx(hKey, NULL, NULL, &dwDisp, (BYTE*)value, &data_size);

        if (query_result == ERROR_SUCCESS) {
            cout << "UniqueID: " << value << endl;
            return value;
        } else {
            cout << "Error querying value: " << result << endl;
        }
    } else {
        cout << "Error opening Registry Key: " << result << endl;
    }

    return NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
