#include <math.h>
#include <minwindef.h>
#include <stdio.h>
#include <string.h>
#include <windef.h>
#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>

// Global variable
HINSTANCE hInst;
UINT MessageCount = 0;
UINT Count = 0;
int posX = 0;
int posY = 0;
int BposX = 400;
int BposY = 400;
BOOL even;

int R = 50;
int G = 200;
int B = 150;

#define INCREMENT(x) x = min(x + 10, 255)
#define DECREMENT(x) x = max(x - 10, 0)

// Function prototypes.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
void paintObject(HWND hWnd, HDC hDC, PAINTSTRUCT ps, int posX, int posY, POINT cursorPosition);
void paintPosition(HWND hWnd, HDC hDC, PAINTSTRUCT ps);

// Application entry point. This is the same as main() in standart C.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;
    BOOL bRet;
    WNDCLASS wcx; // register class
    HWND hWnd;

    hInst = hInstance; // Save the application-instance handle.
                       // Fill in the window class structure with parameters that describe the main
                       // window.

    wcx.style = CS_HREDRAW | CS_VREDRAW;             // redraw if size changes
    wcx.lpfnWndProc = (WNDPROC)MainWndProc;          // points to window procedure
    wcx.cbClsExtra = 0;                              // no extra class memory
    wcx.cbWndExtra = 0;                              // no extra window memory
    wcx.hInstance = hInstance;                       // handle to instance
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);     // predefined app. icon
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);       // predefined arrow
    wcx.hbrBackground = GetStockObject(WHITE_BRUSH); // white background brush
    wcx.lpszMenuName = (LPCSTR) "MainMenu";          // name of menu resource
    wcx.lpszClassName = (LPCSTR) "MainWClass";       // name of window class

    // Register the window class.

    if (!RegisterClass(&wcx))
        return FALSE;

    // create window of registered class

    hWnd = CreateWindow("MainWClass",        // name of window class
                        "ITU",               // title-bar string
                        WS_OVERLAPPEDWINDOW, // top-level window
                        200,                 // default horizontal position
                        25,                  // default vertical position
                        1000,                // default width
                        700,                 // default height
                        (HWND)NULL,          // no owner window
                        (HMENU)NULL,         // use class menu
                        hInstance,           // handle to application instance
                        (LPVOID)NULL);       // no window-creation data
    if (!hWnd)
        return FALSE;

    // Show the window and send a WM_PAINT message to the window procedure.
    // Record the current cursor position.

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // loop of message processing
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            // handle the error and possibly exit
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK MainWndProc(HWND hWnd,     // handle to window
                             UINT uMsg,     // message identifier
                             WPARAM wParam, // first message parameter
                             LPARAM lParam) // second message parameter
{
    HDC hDC;
    PAINTSTRUCT ps;
    POINT cursorPosition;

    // init cursor position
    GetCursorPos(&cursorPosition);
    ScreenToClient(hWnd, &cursorPosition);

    int increment = 10;

    switch (uMsg) {
    case WM_CREATE:
        break;

    // character input
    case WM_CHAR:
        switch (wParam) {
        case 0x08: // backspace
        case 0x0A: // linefeed
        case 0x1B: // escape
            break;

        case 0x09: // tab
            break;

        default:
            break;
        }
        break;

    // key input
    case WM_KEYDOWN:
        switch (wParam) {
        // update posX and posY in order to move object
        case VK_LEFT: // left arrow
            DECREMENT(R);
            BposX -= increment;
            break;
        case VK_RIGHT: // right arrow
            INCREMENT(R);
            BposX += increment;
            break;
        case VK_UP: // up arrow
            INCREMENT(G);
            BposY -= increment;
            break;
        case VK_DOWN: // down arrow
            DECREMENT(G);
            BposY += increment;
            break;

        // react on the other pressed keys
        case VK_SPACE: // space
            INCREMENT(B);
            break;
        case VK_BACK: // backspace
            DECREMENT(B);
            break;
        case VK_TAB: // tab
            break;
            // more virtual codes can be found here:
            // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    // get cursor position
    case WM_MOUSEMOVE:
        // GetCursorPos(&cursorPosition);
        // posX = GET_X_LPARAM(lParam);
        // posY = GET_Y_LPARAM(lParam);
        // InvalidateRect(hWnd, NULL, TRUE);
        break;

    // react on mouse clicks
    case WM_LBUTTONDOWN:
        GetCursorPos(&cursorPosition);
        ScreenToClient(hWnd, &cursorPosition);
        posX = cursorPosition.x;
        posY = cursorPosition.y;
        BposX = cursorPosition.x;
        BposY = cursorPosition.y;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_LBUTTONUP:
        break;

    case WM_RBUTTONDOWN:
        even = !even;
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    // paint objects
    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        paintObject(hWnd, hDC, ps, posX, posY, cursorPosition);
        paintPosition(hWnd, hDC, ps);
        // paint other objects
        // paintObject2(hWnd, hDC, ps, posX, posY, cursorPosition);
        // paintObject3(hWnd, hDC, ps, posX, posY, cursorPosition);
        EndPaint(hWnd, &ps);
        DeleteDC(hDC);
        break;

        //
        // Process other messages.
        //

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

void paintObject(HWND hWnd, HDC hDC, PAINTSTRUCT ps, int posX, int posY, POINT cursorPosition) {
    // Paint rectangles, ellipses, polygons, lines etc.

    // int sizeX = 30;
    // int sizeY = 60;
    //
    // Rectangle(hDC, posX - sizeX, posY - sizeY, posX + sizeX, posY + sizeY);
    //
    // Ellipse(hDC, 200, 200, 300, 400);
    //
    // MoveToEx(hDC, 300, 300, NULL);
    // LineTo(hDC, 400, 300);
    // POINT polygon[] = {
    //     {.x = 500, .y = 100}, {.x = 700, .y = 130}, {.x = 630, .y = 460},
    //     // {.x = 100, .y = 130},
    // };
    // Polygon(hDC, polygon, 3);
    //
    // HPEN hPen = CreatePen(PS_SOLID, 3, RGB(R, G, B));
    // HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
    // HBRUSH hBrush = CreateSolidBrush(RGB(R, G, B));
    // HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
    // Rectangle(hDC, 50, 50, 200, 150); // Coordinates for the rectangle
    // SelectObject(hDC, oldPen);
    // SelectObject(hDC, oldBrush);
    // DeleteObject(hPen);

    ///////////////////////////////////////////////////

    int startwidth = 50;

    MoveToEx(hDC, BposX - startwidth, BposY, NULL);
    LineTo(hDC, BposX - 10, BposY + 100);
    MoveToEx(hDC, BposX + startwidth, BposY, NULL);
    LineTo(hDC, BposX + 10, BposY + 100);

    BOOL even2 = even;
    for (int width = startwidth; width > 0; width -= 20) {
        even2 = !even2;
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
        HBRUSH oldBrush;
        if (even2) {
            oldBrush = (HBRUSH)SelectObject(hDC, redBrush);
        } else {
            oldBrush = (HBRUSH)SelectObject(hDC, blueBrush);
        }
        Ellipse(hDC, BposX - width, BposY + 50, BposX + width, BposY - 50);
        SelectObject(hDC, oldBrush);
    }

    HBRUSH brownBrush = CreateSolidBrush(RGB(139, 69, 19));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, brownBrush);
    Rectangle(hDC, BposX - 10, BposY + 100, BposX + 10, BposY + 120);
    SelectObject(hDC, oldBrush);

    return;
}

void paintPosition(HWND hWnd, HDC hDC, PAINTSTRUCT ps) {
    char text[40]; // buffer to store an output text
    HFONT font;    // new large font
    HFONT oldFont; // saves the previous font

    font = CreateFont(25, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, 0);
    oldFont = (HFONT)SelectObject(hDC, font);
    sprintf(text, "Position -- x:%d, y:%d", posX, posY);
    TextOut(hDC, 50, 600, text, (int)strlen(text));
    SelectObject(hDC, oldFont);
    DeleteObject(font);
}
