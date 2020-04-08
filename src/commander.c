#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include "error_codes.h"
#include "commander.h"
#include "board.h"
#include "hard_coded_config.h"
#include "board_analyzer.h"

#define SLEEP_RAISE_WINDOW_MILISECONDS 700
#define SLEEP_TO_SCREEN_UPDATE_MILISECONDS 200
#define BITMAP_INFORMATION_BIT_COUNT 32
#define BOARD_CELL_SIZE 16
#define BOARD_X_MARGIN 18
#define BOARD_Y_MARGIN 61
#define POINT_OUT_OF_BOARD {-1, -1}
#define POINT_OPEN_LEVEL_MENU {5, -2}

t_error_code raise_minesweeper() {
    STARTUPINFO startup_information;
    PROCESS_INFORMATION process_information;
    ZeroMemory(&startup_information, sizeof(startup_information));
    startup_information.cb = sizeof(startup_information);
    ZeroMemory(&process_information, sizeof(process_information));
    if (!CreateProcess(MINESWEEPER_PATH,
                       NULL, // Command line.
                       NULL, // Process handle not inheritable.
                       NULL, // Thread handle not inheritable.
                       FALSE, // Set handle inheritance to FALSE.
                       0, // No creation flags.
                       NULL, // Use parents environment block.
                       NULL, // Use parents starting directory.
                       &startup_information,
                       &process_information))
        return ERROR_RAISE_MINESWEEPER_CREATE_PROCESS_FAILED;
    Sleep(SLEEP_RAISE_WINDOW_MILISECONDS);
    return RETURN_CODE_SUCCESS;
}

t_error_code move_cursor_out_of_board() {
    POINT point = POINT_OUT_OF_BOARD;
    HWND window_handle = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    if (!window_handle)
        return ERROR_MOVE_CURSOR_OUT_OF_BOARD_FIND_WINDOW_FAILED;
    if (!ClientToScreen(window_handle, &point))
        return ERROR_MOVE_CURSOR_OUT_OF_BOARD_CLIENT_TO_SCREEN_FAILED;
    if (!SetCursorPos(point.x, point.y))
        return ERROR_MOVE_CURSOR_OUT_OF_BOARD_SET_CURSOR_POS_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code click(POINT *cursor_position, bool is_right_click) {
    HWND window_handle = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    if (!window_handle)
        return ERROR_CLICK_FIND_WINDOW_FAILED;
    /*
     * TODO - Think if needed, and if the answer is yes, solve this issue
    if (!SetActiveWindow(window_handle))
        return ERROR_CLICK_SET_ACTIVE_WINDOW_FAILED;
    */
    if (!SetCursorPos(cursor_position->x, cursor_position->y))
        return ERROR_CLICK_SET_CURSOR_POS;
    if (is_right_click) {
        mouse_event(MOUSEEVENTF_RIGHTDOWN, cursor_position->x, cursor_position->y, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, cursor_position->x, cursor_position->y, 0, 0);
    } else {
        mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_position->x, cursor_position->y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, cursor_position->x, cursor_position->y, 0, 0);
    }
    return RETURN_CODE_SUCCESS;
}

void get_minesweeper_cursor_position(POINT *minesweeper_cursor_position, t_board_cell move) {
    minesweeper_cursor_position->x = round(BOARD_X_MARGIN + (move._x * BOARD_CELL_SIZE));
    minesweeper_cursor_position->y = round(BOARD_Y_MARGIN + (move._y * BOARD_CELL_SIZE));
}

t_error_code translate_minesweeper_point_to_screen(POINT *minesweeper_cursor_position) {
    HWND window_handle = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    if (!window_handle)
        return ERROR_TRANSLATE_MINESWEEPER_POINT_TO_SCREEN_FIND_WINDOW_FAILED;
    if (!ClientToScreen(window_handle, minesweeper_cursor_position))
        return ERROR_TRANSLATE_MINESWEEPER_POINT_TO_SCREEN_CLIENT_TO_SCREEN_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code get_screen_cursor_position(POINT *screen_cursor_position, t_board_cell move) {
    get_minesweeper_cursor_position(screen_cursor_position, move);
    t_error_code error_code = translate_minesweeper_point_to_screen(screen_cursor_position);
    if (error_code)
        return error_code;
    return RETURN_CODE_SUCCESS;
}

t_error_code execute_move(t_move move) {
    POINT cursor_position = {0, 0};
    t_error_code error_code = get_screen_cursor_position(&cursor_position, move._cell);
    if (error_code)
        return error_code;
    error_code = click(&cursor_position, move._is_mine);
    if (error_code)
        return error_code;
    error_code = move_cursor_out_of_board();
    if (error_code)
        return error_code;
    Sleep(SLEEP_TO_SCREEN_UPDATE_MILISECONDS);
    return RETURN_CODE_SUCCESS;
}

t_error_code set_minesweeper_level(t_level level) {
    POINT set_level_menu = POINT_OPEN_LEVEL_MENU;
    POINT select_level_button = {level.x_button, level.y_button};
    t_error_code error_code = translate_minesweeper_point_to_screen(&set_level_menu);
    if (error_code)
        return error_code;
    error_code = translate_minesweeper_point_to_screen(&select_level_button);
    if (error_code)
        return error_code;
    error_code = click(&set_level_menu, false);
    if (error_code)
        return error_code;
    error_code = click(&select_level_button, false);
    if (error_code)
        return error_code;
    return RETURN_CODE_SUCCESS;
}

void debug_save_bmp(BYTE *pixels, int width, int height, BITMAPINFOHEADER bitmap_information) {
    // TODO: Delete after development is finished
    FILE *f;
    f = fopen("filename.bmp", "w");
    BITMAPFILEHEADER hdr = {*(WORD *) "BM", 54 + bitmap_information.biSizeImage, 0, 0, 54};
    fwrite((char *) &hdr, 14, 1, f);
    fwrite((char *) &bitmap_information, 40, 1, f);
    fwrite((char *) pixels, 4 * width * height, 1, f);
    fclose(f);
}

t_error_code get_minesweeper_screenshot(t_screenshot_data * screenshot_data_ptr)
{
    //TODO: reduce length by creating sub-functions
    //TODO: Fix cleanup label and verify everything is released (some objects are not).
    t_error_code error_code = RETURN_CODE_SUCCESS;
    RECT window_rect = {0};
    HWND window_handle = FindWindow(0, MINESWEEPER_WINDOW_NAME);
    if (!window_handle)
        return ERROR_GET_MINESWEEPER_SCREENSHOT_FIND_WINDOW_FAILED;
    if(!GetWindowRect(window_handle, &window_rect))
        return ERROR_GET_MINESWEEPER_SCREENSHOT_GET_WINDOW_RECT_FAILED;
    HDC window_dc = GetDC(window_handle);
    HDC desktop_dc = GetDC(HWND_DESKTOP);
    if (!desktop_dc || !window_dc)
        return ERROR_GET_MINESWEEPER_SCREENSHOT_GET_DC_FAILED;
    int width = window_rect.right - window_rect.left;
    int height = window_rect.bottom - window_rect.top;
    HDC compatible_dc = CreateCompatibleDC(desktop_dc);
    if (!compatible_dc)
        return ERROR_GET_MINESWEEPER_SCREENSHOT_CREATE_COMPATIBLE_DC_FAILED;
    HBITMAP hbitmap = CreateCompatibleBitmap(desktop_dc, width, height);
    if (!hbitmap)
        return ERROR_GET_MINESWEEPER_SCREENSHOT_CREATE_COMPATIBLE_BITMAP_FAILED;
    HGDIOBJ oldbmp = SelectObject(compatible_dc, hbitmap);
    if (!oldbmp)
        return ERROR_GET_MINESWEEPER_SCREENSHOT_SELECT_OBJECT_FAILED;
    if (!BitBlt(compatible_dc, 0, 0, width, height, desktop_dc, window_rect.left, window_rect.top, SRCCOPY | CAPTUREBLT))
        return ERROR_GET_MINESWEEPER_SCREENSHOT_BIT_BLT_FAILED;
    if(!SelectObject(compatible_dc, oldbmp))
        return ERROR_GET_MINESWEEPER_SCREENSHOT_SELECT_OBJECT_FAILED;
    if (!DeleteDC(compatible_dc))
        return ERROR_GET_MINESWEEPER_SCREENSHOT_DELETE_DC_FAILED;
    BYTE* pixels = (BYTE *) malloc(height * width * PIXEL_SIZE_IN_BYTES);
    if(!pixels)
    {
        error_code = ERROR_GET_MINESWEEPER_SCREENSHOT_MEMORY_ALLOC_FAILED;
        goto lblCleanup;
    }
    BITMAPINFOHEADER bitmap_information = {sizeof(bitmap_information), width, height, 1, BITMAP_INFORMATION_BIT_COUNT};
    if(!GetDIBits(desktop_dc, hbitmap, 0, height, pixels,
              (BITMAPINFO*)&bitmap_information, DIB_RGB_COLORS))
    {
        error_code = ERROR_GET_MINESWEEPER_SCREENSHOT_GET_DIBITS_FAILED;
        goto lblCleanup;
    }
    debug_save_bmp(pixels, width, height, bitmap_information);
    screenshot_data_ptr->width = width;
    screenshot_data_ptr->height = height;
    screenshot_data_ptr->pixels = pixels;
lblCleanup:
    if(!DeleteObject(hbitmap))
        return ERROR_GET_MINESWEEPER_SCREENSHOT_DELETE_OBJECT_FAILED;
    if(!ReleaseDC(HWND_DESKTOP, window_dc))
        return ERROR_GET_MINESWEEPER_SCREENSHOT_RELEASEDC_FAILED;
    return error_code;
}
