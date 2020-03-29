#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "commander.h"

void raise_app()
{
    STARTUPINFO startup_information;
    PROCESS_INFORMATION process_information;
    ZeroMemory(&startup_information, sizeof(startup_information));
    startup_information.cb = sizeof(startup_information);
    ZeroMemory(&process_information, sizeof(process_information));
    // Start the child process.
    if (!CreateProcess(MINESWEEPER_PATH, // module name
                       NULL, // Command line.
                       NULL, // Process handle not inheritable.
                       NULL, // Thread handle not inheritable.
                       FALSE, // Set handle inheritance to FALSE.
                       0, // No creation flags.
                       NULL, // Use parents environment block.
                       NULL, // Use parents starting directory.
                       &startup_information, // Pointer to STARTUPINFO structure.
                       &process_information)) // Pointer to PROCESS_INFORMATION structure)
        goto l_app_not_raised;
    Sleep(SLEEP_RAISE_WINDOW_MILISECONDS);
    // Verify the window has been raised.
    HWND hWnd = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    if (hWnd == NULL)
        goto l_app_not_raised;
    return;
l_app_not_raised:
    printf(APP_NOT_RAISED_MESSAGE);
    exit(1);
}

void click(POINT cursor_position, bool is_right_click)
{
    HWND hWnd = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    SetActiveWindow(hWnd);
    if(!SetCursorPos(cursor_position.x, cursor_position.y))
    {
        printf(MOUSE_PROBLEM_MESSAGE);
        exit(1);
    }
    if (is_right_click)
    {
        mouse_event(MOUSEEVENTF_RIGHTDOWN, cursor_position.x, cursor_position.y, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, cursor_position.x, cursor_position.y, 0, 0);
    }
    else
    {
        mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_position.x, cursor_position.y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, cursor_position.x, cursor_position.y, 0, 0);
    }
}

POINT get_screen_cursor_position(t_board_cell move)
{
    POINT point = get_app_cursor_position(move);
    HWND hWnd = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    ClientToScreen(hWnd, &point);
    return point;
}

POINT get_app_cursor_position(t_board_cell move)
{
    POINT point;
    point.x = round(BOARD_X_MARGIN + (move._x * BOARD_CELL_SIZE));
    point.y = round(BOARD_Y_MARGIN + (move._y * BOARD_CELL_SIZE));
    return point;
}

void execute_move(t_move move)
{
    POINT cursor_position = get_screen_cursor_position(move._cell);
    click(cursor_position, move._is_mine);
}

void set_level_in_app()
{
    //TODO Implement
    if (game_level_mark == EXPERT_MARK)
        return;
    else if (game_level_mark == INTERMEDIATE_MARK)
        return;
    else
        return;
}

HBITMAP get_app_screenshot_bitmap()
{
    RECT client_rect = {0};
    HWND window_handle = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    GetClientRect(window_handle, &client_rect);
    GetWindowRect(window_handle, &client_rect);
    HDC hdcScreen = GetDC(window_handle);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;
    HBITMAP bitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    BitBlt(hdc, 0, 0, width, height, hdcScreen, client_rect.left, client_rect.top, SRCCOPY);
    ReleaseDC(HWND_DESKTOP, hdcScreen);
    return bitmap;
}

BYTE * get_app_screenshot()
{
    HBITMAP bitmap = get_app_screenshot_bitmap();
    HDC hdcSource = NULL;
    HBITMAP hSource = NULL;
    BITMAPINFO bitmap_info = {0};
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    if(0 == GetDIBits(hdcSource, hSource, 0, 0, NULL, &bitmap_info, DIB_RGB_COLORS))
        goto l_unexpected_event;
    BYTE* lpPixels = (BYTE *) malloc(bitmap_info.bmiHeader.biSizeImage);

    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;  // no compression -> easier to use
    bitmap_info.bmiHeader.biHeight = abs(bitmap_info.bmiHeader.biHeight);
    if(0 == GetDIBits(hdcSource, hSource, 0, bitmap_info.bmiHeader.biHeight,
                      lpPixels, &bitmap_info, DIB_RGB_COLORS))
    {
        free(lpPixels);
        goto l_unexpected_event;
    }
    DeleteObject(bitmap);
    return lpPixels;

l_unexpected_event:
    DeleteObject(bitmap);
    exit(1);
}

