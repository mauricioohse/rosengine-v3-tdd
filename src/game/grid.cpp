#include "grid.h"
#include "engine_constants.h"
#include "engine.h"
#include "window.h"
#include "input.h"
#include "stdio.h"
#include "string.h"

#define DEBUG_DRAW_HOVERED_SQUARE 1


// constants - change here to resize the grid and the squares
static int GRID_MAX_WIDTH = (int) (WINDOW_WIDTH*0.80); // in fraction of screen
static int GRID_MAX_HEIGHT = (int) (WINDOW_HEIGHT*0.80);
int Grid::GRID_SQUARE_LENGTH = 48;
static int GRID_HORIZONTAL_SQUARE_COUNT =  (int) GRID_MAX_WIDTH/Grid::GRID_SQUARE_LENGTH;
static int GRID_VERTICAL_SQUARE_COUNT = (int)  GRID_MAX_HEIGHT/Grid::GRID_SQUARE_LENGTH;
static int GRID_CLAMPED_WIDTH = Grid::GRID_SQUARE_LENGTH*GRID_HORIZONTAL_SQUARE_COUNT;
static int GRID_CLAPMED_HEIGHT = Grid::GRID_SQUARE_LENGTH*GRID_VERTICAL_SQUARE_COUNT;

// note: X,Y start at the top left (to keep SDL standard)
typedef struct  {
    int LoadedGrid[100][100];
    int maxX;
    int maxY;
    bool isLoaded;
} LoadedLevel_t;

static LoadedLevel_t currLevel;


static Point monster_path[100] = {0,0};
static int monster_path_size = 0;

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

    // paint the path that monsters walk on - it starts at number 1 until the maximum number.
    SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 0, 0, 50);
    for (int i = 0; i < monster_path_size - 1; i++) {
        SDL_RenderDrawLine(g_Engine.window->renderer, 
                          monster_path[i].x, monster_path[i].y,
                          monster_path[i+1].x, monster_path[i+1].y);
    }
    
    // draw path points
    SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 255, 0, 255);
    for (int i = 0; i < monster_path_size; i++) {
        SDL_Rect point_rect = {monster_path[i].x - 2, monster_path[i].y - 2, 4, 4};
        SDL_RenderFillRect(g_Engine.window->renderer, &point_rect);
    }

}

bool Grid::IsInsideGrid(int x, int y)
{
    return (x > GRID_START_POINT.x && 
            x < GRID_END_POINT.x && 
            y > GRID_START_POINT.y && 
            y < GRID_END_POINT.y);
}

static bool CreateMonsterPath()
{
    // based on the loaded grid, create the monster path
    monster_path_size = 0;
    
    // find starting position (value 1)
    int start_row = -1, start_col = -1;
    for (int row = 0; row < currLevel.maxY; row++) {
        for (int col = 0; col < currLevel.maxX; col++) {
            // printf("checking grid[%d][%d] = %d\n", row, col, currLevel.LoadedGrid[row][col]);
            if (currLevel.LoadedGrid[row][col] == 1) {
                start_row = row;
                start_col = col;
                break;
            }
        }
        if (start_row != -1) break;
    }
    
    if (start_row == -1) {
        printf("no starting position found in level\n");
        return false;
    }
    
    // trace path from 1 to maximum value
    int current_value = 1;
    int current_row = start_row;
    int current_col = start_col;
    int last_value = 1;
    
    while (monster_path_size < 100) {
        // convert grid coordinates to screen coordinates
        Point screen_pos = Grid::GridToScreenPosition(current_col, current_row);
        monster_path[monster_path_size].x = screen_pos.x + Grid::GRID_SQUARE_LENGTH/2;
        monster_path[monster_path_size].y = screen_pos.y + Grid::GRID_SQUARE_LENGTH/2;
        monster_path_size++;
        
        last_value = current_value;
        
        // find next value
        current_value++;
        bool found_next = false;
        
        // check adjacent cells for next value
        int directions[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
        for (int i = 0; i < 4; i++) {
            int next_row = current_row + directions[i][0];
            int next_col = current_col + directions[i][1];
            
            if (next_row >= 0 && next_row < currLevel.maxY && 
                next_col >= 0 && next_col < currLevel.maxX &&
                currLevel.LoadedGrid[next_row][next_col] == current_value) {
                current_row = next_row;
                current_col = next_col;
                found_next = true;
                break;
            }
        }
        
        if (!found_next) break;
    }
    
    printf("monster path created with %d points, last value: %d\n", monster_path_size, last_value);
    return true;
}

bool Grid::LoadLevel(const char *filename)
{
    char path[100];
    path[0] = '\0';
    strncat(path, "assets/levels/", sizeof(path) - 1);
    strncat(path, filename, sizeof(path) - strlen(path) - 1);

    FILE * fp = fopen(path, "r");

    memset(&currLevel, 0, sizeof(currLevel));

    if (fp)
    {
        char line[256];
        int row = 0;
        
        while (fgets(line, sizeof(line), fp) && row < GRID_VERTICAL_SQUARE_COUNT)
        {
            int col = 0;
            char *token = line;
            char *end;
            
            while (*token && col < GRID_HORIZONTAL_SQUARE_COUNT)
            {
                // skip whitespace
                while (*token == ' ' || *token == '\t') token++;
                
                if (*token == '\0' || *token == '\n') break;
                
                int value = (int)strtol(token, &end, 10);
                
                if (end == token)
                {
                    printf("loading level: invalid value at row %d, col %d\n", row, col);
                    fclose(fp);
                    return false;
                }
                
                currLevel.LoadedGrid[row][col] = value;
                printf("%02d", value);
                if (col < GRID_HORIZONTAL_SQUARE_COUNT - 1) printf(",");
                col++;
                
                token = end;
                if (*token == ',') token++;
            }
            printf("\n");
            row++;

            
            currLevel.maxX = col;
            currLevel.maxY = row;
        }

        
        fclose(fp);
        printf("Successfully loaded csv level %s\n", filename);

        if(!CreateMonsterPath())
        {
            return false;
        }

        currLevel.isLoaded = true;

        return true;
    } 
    else
    {
        printf("Couldnt open %s: Fail to load csv level\n", path);
    }
    

    return false;
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

Point Grid::GridToScreenPosition(int grid_x, int grid_y)
{
    return Point{
        GRID_START_POINT.x + grid_x * GRID_SQUARE_LENGTH,
        GRID_START_POINT.y + grid_y * GRID_SQUARE_LENGTH
    };
}

Point Grid::GetNearestGridPointCenter(int x, int y)
{
    Point corner = GetNearestGridPoint(x,y);
    Point center = corner + Point{Grid::GRID_SQUARE_LENGTH/2, Grid::GRID_SQUARE_LENGTH/2};
    return center;
}

Point Grid::GetMonsterPathPoint(int index)
{
    if (index >= 0 && index < monster_path_size) {
        return monster_path[index];
    }
    return Point{0, 0}; // return default if index out of bounds
}

int Grid::GetMonsterPathSize()
{
    return monster_path_size;
}
