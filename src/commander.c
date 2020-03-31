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
    SetCursorPos(cursor_position.x, cursor_position.y);
    if(is_right_click)
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
    move_cursor_out_of_board();
    Sleep(SLEEP_TO_SCREEN_UPDATE_MILISECONDS);
}

void set_level_in_app()
{
    if (game_level_mark == EXPERT_MARK)
        return;
    else if (game_level_mark == INTERMEDIATE_MARK)
        return;
    else
        return;
}

void move_cursor_out_of_board()
{
    POINT point = {POINT_OUT_OF_BOARD_X, POINT_OUT_OF_BOARD_Y};
    HWND hWnd = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    ClientToScreen(hWnd, &point);
    SetCursorPos(point.x, point.y);
}

t_screenshot_data get_app_screenshot()
{
    t_screenshot_data screenshot_data;
    RECT window_rect = {0};
    HWND window_handle = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    GetWindowRect(window_handle, &window_rect);
    HDC hdcScreen = GetDC(window_handle);
    HDC hdc = GetDC(HWND_DESKTOP);
    int width = window_rect.right - window_rect.left;
    int height = window_rect.bottom - window_rect.top;
    HDC dc = CreateCompatibleDC(hdc);
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height);
    HGDIOBJ oldbmp = SelectObject(dc, hbitmap);
    BitBlt(dc, 0, 0, width, height, hdc, window_rect.left, window_rect.top, SRCCOPY | CAPTUREBLT);
    SelectObject(dc, oldbmp);
    DeleteDC(dc);
    BYTE* pixels = (BYTE *) malloc (height * width * 4);
    BITMAPINFOHEADER bitmap_information = {sizeof(bitmap_information), width, height, 1, BITMAP_INFORMATION_BIT_COUNT};
    GetDIBits(hdc, hbitmap, 0, height, pixels,
              (BITMAPINFO*)&bitmap_information, DIB_RGB_COLORS);
    DeleteObject(hbitmap);
    ReleaseDC(HWND_DESKTOP, hdcScreen);
    //debug_save_bmp(pixels, width, height, bitmap_information);
    screenshot_data.width = width;
    screenshot_data.height = height;
    screenshot_data.pixels = pixels;
    return screenshot_data;
}

void debug_save_bmp(BYTE * pixels, int width, int height, BITMAPINFOHEADER bitmap_information)
{
    // TODO: Delete after development is finished
    FILE * f;
    f = fopen("filename.bmp", "w");
    BITMAPFILEHEADER hdr = {'MB', 54 + bitmap_information.biSizeImage, 0, 0, 54};
    fwrite((char*)&hdr, 14, 1, f);
    fwrite((char*)&bitmap_information, 40, 1, f);
    fwrite((char*)pixels, 4 * width * height, 1, f);
    fclose(f);
}




