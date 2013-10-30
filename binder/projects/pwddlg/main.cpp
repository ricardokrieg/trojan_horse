#include "stdafx.h"

LRESULT CALLBACK	PwdWindow(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
   MSG msg;
   WNDCLASSEX wcex;
   HWND hWnd = NULL;
   HWND hEdit = NULL;

   (void) memset( &wcex, 0x00, sizeof(WNDCLASSEX) );

   wcex.cbSize        = sizeof(WNDCLASSEX);
   wcex.style			 = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc	 = PwdWindow;
   wcex.hCursor		 = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+2);
   wcex.lpszClassName = "@PWDWIN@";

	RegisterClassEx(&wcex);

   hWnd = CreateWindow( "@PWDWIN@", 
                        " Type the password ...", 
                        WS_OVERLAPPED, 
                        GetSystemMetrics(SM_CXSCREEN)/2-100, 
                        GetSystemMetrics(SM_CYSCREEN)/2-75, 
                        200, 150, 
                        NULL, NULL, NULL, NULL);
   if (!hWnd)
      return 0;

   CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_TEXT, 10, 80, 70, 30, hWnd, (HMENU)10123, NULL, NULL); 
   CreateWindow("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_TEXT, 110, 80, 70, 30, hWnd, (HMENU)10456, NULL, NULL); 
   hEdit = CreateWindow("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL, 10, 20, 170, 25, hWnd, (HMENU)10789, NULL, NULL);

   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);
   
   SetFocus(hEdit);

   while ( GetMessage(&msg, NULL, 0, 0) )
   {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
   }

	if ( (int)msg.wParam == 0 )
      exit(0);

   DestroyWindow(hWnd);

   return 0;
}


LRESULT CALLBACK PwdWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId = -1;
   char pwd[32];

	switch (message)
	{
		case WM_COMMAND:
      {   
			wmId = LOWORD(wParam);

         switch (wmId)
         {
            case 10123:
            {
               (void) memset( pwd, 0x00, sizeof(pwd) );
               GetWindowText( GetDlgItem(hWnd, 10789), pwd, 32 );
               if ( strcmp( pwd, "PASSWORD" ) )
               {
                  MessageBox( hWnd, "Sorry! Wrong password.", "Password", MB_ICONERROR );
                  PostQuitMessage(0);
               }
               else
                  PostQuitMessage(1);
            }                 
              break;

            case 10456:
               PostQuitMessage(0);
               break;

            default:
               break;
         }
      }
		  break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

