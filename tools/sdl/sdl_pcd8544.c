/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "sdl_pcd8544.h"
#include "sdl_oled_basic.h"

static int s_activeColumn = 0;
static int s_activePage = 0;
const static int s_columnStart = 0;
const static int s_columnEnd = 83;
const static int s_pageStart = 0;
const static int s_pageEnd = 5;
static int s_verticalMode = 0;

static int sdl_pcd8544_detect(uint8_t data)
{
    return (data == 0x04);
}

static void sdl_pcd8544_commands(uint8_t data)
{
//    printf("%02X\n", data);
    switch (s_commandId)
    {
        case 0x20:
            s_verticalMode = 0;
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0x22:
            s_verticalMode = 1;
            s_commandId = SSD_COMMAND_NONE;
            break;
        default:
            if ((s_commandId & 0x80) == 0x80 )
            {
                s_activeColumn = s_commandId & 0x7F;
                if (s_activeColumn > s_columnEnd) s_activeColumn = s_columnEnd;
            }
            if ((s_commandId & 0xC0) == 0x40 )
            {
                s_activePage = s_commandId & 0x3F;
                if (s_activePage > s_pageEnd) s_activePage = s_pageEnd;
            }
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}

void sdl_pcd8544_data(uint8_t data)
{
    int x = s_activeColumn;
    int y = s_activePage;
    for (int i=0; i<8; i++)
    {
        if (data & (1<<i))
        {
            SDL_SetRenderDrawColor( g_renderer, 170, 170, 205, 255 );
        }
        else
        {
            SDL_SetRenderDrawColor( g_renderer, 20, 20, 20, 255 );
        }
        SDL_Rect r;
        r.x = x * PIXEL_SIZE + BORDER_SIZE;
        r.y = ((y<<3) + i) * PIXEL_SIZE + BORDER_SIZE + TOP_HEADER;
        r.w = PIXEL_SIZE;
        r.h = PIXEL_SIZE;
        // Render rect
        SDL_RenderFillRect( g_renderer, &r );
    }
    if (s_verticalMode)
    {
        s_activePage++;
        if (s_activePage > s_pageEnd)
        {
            s_activePage = s_pageStart;
        }
    }
    else
    {
        s_activeColumn++;
        if (s_activeColumn > s_columnEnd)
        {
            s_activeColumn = s_columnStart;
        }
    }
}

sdl_oled_info sdl_pcd8544 =
{
    .width = 84,
    .height = 48,
    .dataMode = SDMS_AUTO,
    .detect = sdl_pcd8544_detect,
    .run_cmd = sdl_pcd8544_commands,
    .run_data = sdl_pcd8544_data,
};
