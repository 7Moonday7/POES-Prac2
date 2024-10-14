//	Prac2.cpp
//
#define  STRICT
#define  WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI  WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE, // hPrevInstance,
    _In_ LPSTR, // lpCmdLine,
    _In_ int nCmdShow
)
{

    LPCTSTR szClass = TEXT("a;eohgqeruiopugoqeg");

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    ::RegisterClass(&wc);


    // Create Main window
    HWND hWnd = ::CreateWindow(szClass, "General", WS_OVERLAPPEDWINDOW,
        0, 0, 400, 200, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL) {
        return -1;
    }

    ::ShowWindow(hWnd, nCmdShow);
    // ::UpdateWindow(hWnd);

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) { // idle
        ::DispatchMessage(&msg);
    }

    return 0;
}
//=========================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================