//	Prac2.cpp
//
#define  STRICT
#define  WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EqualRect(HWND hWnd, HDC hdc);

const int N = 9; //количество прямоугольников 3x3

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

RECT gridRects[9];
int currentRect = -1;

void EqualRect(HWND hWnd, HDC hdc) {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    int cols = 3;
    int raws = 3;

    int width = (clientRect.right - clientRect.left) / cols;
    int height = (clientRect.bottom - clientRect.top) / raws;

    //Рисуем сетку
    HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i < cols; ++i)
    {
        ::MoveToEx(hdc, i * width, 0, NULL);
        ::LineTo(hdc, i * width, clientRect.bottom);
    }
    for (int i = 0; i < raws; ++i)
    {
        ::MoveToEx(hdc, 0, i * height, NULL);
        ::LineTo(hdc, clientRect.right, i * height);
    }

    ::SelectObject(hdc, oPen);
    ::DeleteObject(hPen);

    // Заполнение массива с прямоугольниками
    for (int i = 0; i < raws; ++i) {
        for (int j = 0; j < cols; ++j) {
            gridRects[i * cols + j].left = j * width;
            gridRects[i * cols + j].top = i * height;
            gridRects[i * cols + j].right = (j + 1) * width;
            gridRects[i * cols + j].bottom = (i + 1) * height;
        }
    }
}

void  PtInRect(HWND hWnd, HDC hdc, POINT pt) {
    for (int i = 0; i < N; ++i) {
        if (PtInRect(&gridRects[i], pt)) {
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 0));
            FillRect(hdc, &gridRects[i], hBrush);
            DeleteObject(hBrush);
            currentRect = i; // Запоминаем индекс выделенного прямоугольника
            break;
        }
    }
}

void ClearHighlight(HWND hWnd, HDC hdc) {
    if (currentRect != -1) {
        HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(hdc, &gridRects[currentRect], hBrush);
        DeleteObject(hBrush);
        currentRect = -1; // Сброс выделения
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT lastMousePos = { -1, -1 };
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EqualRect(hWnd, hdc);
        PtInRect(hWnd, hdc, lastMousePos); // Обновление выделения при перерисовке
        EqualRect(hWnd, hdc); //Отрисовка сетки по новой
        EndPaint(hWnd, &ps);
        break;

    case WM_MOUSEMOVE: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (currentRect != -1 && PtInRect(&gridRects[currentRect], pt)) {
            break; // Если курсор уже в текущем выделенном прямоугольнике, ничего не делаем
        }
        hdc = GetDC(hWnd);
        ClearHighlight(hWnd, hdc); // Очистка старого выделения
        lastMousePos = pt;
        PtInRect(hWnd, hdc, pt); // Новое выделение
        ReleaseDC(hWnd, hdc);
    }
    break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================