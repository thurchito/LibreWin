/*
 * Free95
 *
 * Module name:
 *      progman.c
 *
 * Description:
 *      Program Manager from Free95 for Windows.
 *
 * Author: Kap Petrov
 *
*/

#include <windows.h>

// Window Procedure to be called to handle messages from the Operating System
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	POINT pt = {x, y};

	RECT target = {20, 15, 48, 48};

	PAINTSTRUCT ps;
	HDC hdc;

    switch (uMsg) 
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
            if (PtInRect(&target, pt))
            {
	            MessageBox(0, "Application was not found!", "Error", MB_OK);
            }
            return 0;
		case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 8, 58, "Winver", 6);

	        RECT rect = {20, 15, 48, 48};
	        
	        // Create a solid brush with a specific color
	        HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 255)); // Blue color
	        
	        // Fill the rectangle with the brush
	        FillRect(hdc, &rect, hBrush);
	        
	        // Clean up by deleting the brush
	        DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
			return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Define and register the window class.
    const char CLASS_NAME[] = "ReshellApp";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);
    
    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                    // Window class name.
        "Program Manager",          // Window title.
        WS_OVERLAPPEDWINDOW,           // Window style.

        10, 10, 300, 300, // x, y, width, height
        NULL,                          // Parent window.
        NULL,                          // Menu.
        hInstance,                     // Instance handle.
        NULL                           // Additional application data.
    );

    // Show the window.
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
