#include "grid.h"
#include "engine_constants.h"
#include "engine.h"
#include "window.h"
#include "input.h"

#define DEBUG_DRAW_HOVERED_SQUARE 1

// constants - change here to resize the grid and the squares
static int GRID_MAX_WIDTH = (int) (WINDOW_WIDTH*0.80); // in fraction of screen
static int GRID_MAX_HEIGHT = (int) (WINDOW_HEIGHT*0.80);
static int GRID_SQUARE_LENGTH = 48;
static int GRID_HORIZONTAL_SQUARE_COUNT =  (int) GRID_MAX_WIDTH/GRID_SQUARE_LENGTH;
static int GRID_VERTICAL_SQUARE_COUNT = (int)  GRID_MAX_HEIGHT/GRID_SQUARE_LENGTH;
static int GRID_CLAMPED_WIDTH = GRID_SQUARE_LENGTH*GRID_HORIZONTAL_SQUARE_COUNT;
static int GRID_CLAPMED_HEIGHT = GRID_SQUARE_LENGTH*GRID_VERTICAL_SQUARE_COUNT;


Point Grid::GRID_START_POINT = {20,20};
Point Grid::GRID_END_POINT = {
    GRID_START_POINT.x + GRID_HORIZONTAL_SQUARE_COUNT * GRID_SQUARE_LENGTH,
    GRID_START_POINT.y + GRID_VERTICAL_SQUARE_COUNT *GRID_SQUARE_LENGTH,
};

void Grid::DrawGrid(){

    // set render draw to white
    SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 255, 255, 255);


    // vertical lines
    for (int i = 0; i <= GRID_HORIZONTAL_SQUARE_COUNT; i++)
    {
        int x1 = GRID_START_POINT.x + i * GRID_SQUARE_LENGTH;
        int y1 = GRID_START_POINT.y;
        int x2 = x1;
        int y2 = y1 + GRID_CLAPMED_HEIGHT;
        SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x2, y2);
    }

    // horizontal lines
    for (int j = 0; j <= GRID_VERTICAL_SQUARE_COUNT; j++)
    {
        int x1 = GRID_START_POINT.x;
        int y1 = GRID_START_POINT.y + j * GRID_SQUARE_LENGTH;
        int x2 = x1 + GRID_CLAMPED_WIDTH;
        int y2 = y1;
        SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x2, y2);
    }

    // paint the mouse-hovered square
    if (DEBUG_DRAW_HOVERED_SQUARE)
    {
        int x,y;
        Input::GetMousePosition(x, y);

        
        if (IsInsideGrid(x,y))
        {
            SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 0, 0, 55);
            Point p = GetNearestGridPoint(x,y);
            SDL_Rect rect = {p.x,p.y,GRID_SQUARE_LENGTH,GRID_SQUARE_LENGTH};
            SDL_RenderDrawRect(g_Engine.window->renderer,  &rect);
        }
        else
        {
        }
    }
}

bool Grid::IsInsideGrid(int x, int y)
{
    return (x > GRID_START_POINT.x && 
            x < GRID_END_POINT.x && 
            y > GRID_START_POINT.y && 
            y < GRID_END_POINT.y);
}

// aligned on the top left of the square that contains x and y
Point Grid::GetNearestGridPoint(int x, int y)
{
    // clamps x and y by GRID_SQUARE_LENGTH
    int x_count_offset = (x-GRID_START_POINT.x)/GRID_SQUARE_LENGTH;
    int y_count_offset = (y-GRID_START_POINT.y)/GRID_SQUARE_LENGTH;
    int x_pos = GRID_START_POINT.x + x_count_offset*GRID_SQUARE_LENGTH;
    int y_pos = GRID_START_POINT.y + y_count_offset*GRID_SQUARE_LENGTH;
    return Point{x_pos, y_pos};
}