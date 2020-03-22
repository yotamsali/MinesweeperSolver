#include <stdio.h>
#include "commander.h"

static HANDLE screen_handler;

void raise_app()
{
    screen_handler = GetStdHandle(STD_OUTPUT_HANDLE);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    bool raised_successfully;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    // Start the child process.
    if (!CreateProcess(MINESWEEPER_PATH, // module name
                       NULL, // Command line.
                       NULL, // Process handle not inheritable.
                       NULL, // Thread handle not inheritable.
                       FALSE, // Set handle inheritance to FALSE.
                       0, // No creation flags.
                       NULL, // Use parents environment block.
                       NULL, // Use parents starting directory.
                       &si, // Pointer to STARTUPINFO structure.
                       &pi)) // Pointer to PROCESS_INFORMATION structure)
    {
        printf(APP_NOT_RAISED_MESSAGE);
        exit(1);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
}

void click(COORD cursor_position, bool is_right_click)
{
    SetConsoleCursorPosition(screen_handler, cursor_position);
    if (is_right_click)
    {
        mouse_event(MOUSEEVENTF_RIGHTDOWN, cursor_position.X, cursor_position.Y, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, cursor_position.X, cursor_position.Y, 0, 0);
    }
    else
    {
        mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_position.X, cursor_position.Y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, cursor_position.X, cursor_position.Y, 0, 0);
    }
}

COORD get_cursor_position(t_board_cell move)
{
    COORD cursor_position;
    cursor_position.X = 0;
    cursor_position.Y = 0;
    return cursor_position;
}

void execute_move(t_move move)
{
    COORD cursor_position = get_cursor_position(move._cell);
    click(cursor_position, move._is_mine);
}

void set_level()
{
    if (game_level == EXPERT_MARK)
        return;
    else if (game_level == INTERMEDIATE_MARK)
        return;
    else
        return;
}

void set_cursor_to_default()
{
    return;
}


HBITMAP get_app_screenshot()
{
    RECT client_rect = {0};
    LPSTR app_name = MINESWEEPER_PATH;
    HWND window_handle = FindWindow(0, MINESWEEPER_PATH);
    GetClientRect(window_handle, &client_rect);
    GetWindowRect(window_handle, &client_rect);
    HDC hdcScreen = GetDC(window_handle);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width, height);
    BitBlt(hdc, 0, 0, width, height, hdcScreen, client_rect.left, client_rect.top, SRCCOPY);
    ReleaseDC(HWND_DESKTOP, hdcScreen);
    return hbmp;
}

