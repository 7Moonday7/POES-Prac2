//	Prac2.cpp
//
#define  STRICT
#define  WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <Windows.h>

#define N 9 //количество прямоугольников 3x3

struct AppState {
    RECT gridRects[N];
    int currentRect;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EqualRect(HWND hWnd, HDC hdc, AppState* state);
void CheckPtInRect(HWND hWnd, HDC hdc, POINT pt, AppState* state);
void HighlightRect(HDC hdc, int rectIndex, AppState* state);
void ClearHighlight(HWND hWnd, HDC hdc, AppState* state);

int WINAPI  WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE, // hPrevInstance,
    _In_ LPSTR, // lpCmdLine,
    _In_ int nCmdShow
)
{
    AppState state;
    state.currentRect = -1;

    LPCTSTR szClass = TEXT("a;eohgqeruiopugoqeg");

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);


    // Create Main window
    HWND hWnd = CreateWindow(szClass, "General", WS_OVERLAPPEDWINDOW,
        0, 0, 400, 200, NULL, NULL, hInstance, &state);
    if (hWnd == NULL) {
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { // idle
        DispatchMessage(&msg);
    }

    return 0;
}
//=========================================================

void EqualRect(HWND hWnd, HDC hdc, AppState* state) {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    int cols = 3;
    int raws = 3;

    int width = (clientRect.right - clientRect.left) / cols;
    int height = (clientRect.bottom - clientRect.top) / raws;

    //Рисуем сетку
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i < cols; ++i)
    {
        MoveToEx(hdc, i * width, 0, NULL);
        LineTo(hdc, i * width, clientRect.bottom);
    }
    for (int i = 0; i < raws; ++i)
    {
        MoveToEx(hdc, 0, i * height, NULL);
        LineTo(hdc, clientRect.right, i * height);
    }

    SelectObject(hdc, oPen);
    DeleteObject(hPen);

    // Заполнение массива с прямоугольниками
    for (int i = 0; i < raws; ++i) {
        for (int j = 0; j < cols; ++j) {
            state->gridRects[i * cols + j].left   = j * width + 1;
            state->gridRects[i * cols + j].top    = i * height + 1;
            state->gridRects[i * cols + j].right  = (j + 1) * width - 1;
            state->gridRects[i * cols + j].bottom = (i + 1) * height - 1;
        }
    }
}

void HighlightRect(HDC hdc, int rectIndex, AppState* state) {
    if (rectIndex >= 0 && rectIndex < N) {
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 0));
        FillRect(hdc, &state->gridRects[rectIndex], hBrush);
        DeleteObject(hBrush);
    }
}

void  CheckPtInRect(HWND hWnd, HDC hdc, POINT pt, AppState* state) {
    // Если курсор за пределами окна, сбрасываем выделение
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    if (!PtInRect(&clientRect, pt)) {
        ClearHighlight(hWnd, hdc, state);
        return;
    }

    for (int i = 0; i < N; ++i) {
        if (PtInRect(&state->gridRects[i], pt)) {
            HighlightRect(hdc, i, state);
            state->currentRect = i; // Запоминаем индекс выделенного прямоугольника
            break;
        }
    }
}

void ClearHighlight(HWND hWnd, HDC hdc, AppState* state) {
    if (state->currentRect != -1) {
        HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(hdc, &state->gridRects[state->currentRect], hBrush);
        state->currentRect = -1; // Сброс выделения
        EqualRect(hWnd, hdc, state); // Отрисовка сетки по новой
        DeleteObject(hBrush);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static AppState* state = nullptr;
    static POINT lastMousePos = { -1, -1 };
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message) {
    case WM_CREATE: {
        CREATESTRUCT* cs { (CREATESTRUCT*)lParam };

        state = (AppState*)cs->lpCreateParams;

        return 0;
    }
    case WM_SIZING:
    {        
        InvalidateRect(hWnd, nullptr, false);
        return 1;
    }
    case WM_SIZE:
    {
       
        InvalidateRect(hWnd, nullptr, false);
        return 0;
    }
    case WM_ERASEBKGND: {
        return 1;
    }
    case WM_PAINT: {
        hdc = BeginPaint(hWnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
        EqualRect(hWnd, hdc, state); // Отрисовка сетки
        HighlightRect(hdc, state->currentRect, state); // Обновление выделения при перерисовке
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEMOVE: {
        TRACKMOUSEEVENT tme{ };
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 1;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        // Получаем контекст устройства для рисования
        hdc = GetDC(hWnd);

        // Проверка на выход курсора за пределы клиентской области окна
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        if (!PtInRect(&clientRect, pt)) {
            ClearHighlight(hWnd, hdc, state);  // Очистка выделения, если курсор вне окна
            ReleaseDC(hWnd, hdc);  // Освобождаем контекст устройства
            return 0;  // Завершаем обработку события
        }

        // Обновляем выделение, если курсор внутри окна
        ClearHighlight(hWnd, hdc, state);  // Очищаем старое выделение
        CheckPtInRect(hWnd, hdc, pt, state);  // Проверка и выделение нового прямоугольника
        ReleaseDC(hWnd, hdc);  // Освобождаем контекст устройства
    }
    break;
    case WM_MOUSELEAVE: {
        if (state->currentRect != -1) {  // Проверяем, если есть активное выделение
            hdc = GetDC(hWnd);
            ClearHighlight(hWnd, hdc, state);  // Очищаем выделение
            ReleaseDC(hWnd, hdc);
            state->currentRect = -1;  // Сброс выделенного прямоугольника
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================