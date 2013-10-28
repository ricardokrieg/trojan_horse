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

string file_to_string(string filename) {
    string str_file;
    FILE *file;

    file = fopen(filename.c_str(), "rb");

    char ch;
    while(!feof(file)) {
        ch = fgetc(file);

        if(ferror(file)) exit(1);
        if(!feof(file)) str_file += ch;
    }

    if (fclose(file)==EOF) exit(1);

    return str_file;
}

//------------------------------------------------------------------------------

void wait(void) {
    usleep(1000 * 100);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
