#include "sandfall.h"
#include "../core/window.h"
#include "../core/input.h"

#define BLOCK_SIZE 16 // size, in pixels, that each block can have
#define BLOCK_BORDER_SIZE 1 // pixel size of the border of each block
#define VIEW_SIZE 640 // pixel, square 
#define GRID_SIZE VIEW_SIZE/(BLOCK_SIZE) // how many blocks fit in the grid



VP::BLOCK_MATERIALS grid[GRID_SIZE][GRID_SIZE];
VP::BLOCK_MATERIALS next_grid[GRID_SIZE][GRID_SIZE];
VP::BLOCK_MATERIALS selectedMaterial;

viewPort g_viewPort;
SDL_Renderer* renderer;

struct Pos {
    int x;
    int y;

    // sum operator
    Pos operator+(const Pos& other) const {
        return {x + other.x, y + other.y};
    }
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
};

Pos getPosFromDirection(Direction dir) {
    switch (dir) {
        case UP:
            return {0, 1};
        case DOWN:
            return {0, -1};
        case LEFT:
            return {-1, 0};
        case RIGHT:
            return {1, 0};
        case UP_LEFT:
            return {-1, 1};
        case UP_RIGHT:
            return {1, 1};
        case DOWN_LEFT:
            return {-1, -1};
        case DOWN_RIGHT:
            return {1, -1};
    }
}           

Pos getAdjacentPos(Pos pos, Direction dir) {
    return pos + getPosFromDirection(dir);
}

void VP::InitViewPort(){
    renderer = g_Engine.window->renderer;
    g_viewPort.x = ( WINDOW_WIDTH - VIEW_SIZE)/2;
    g_viewPort.y = 80;
    g_viewPort.width = VIEW_SIZE;
    g_viewPort.height = VIEW_SIZE;
    g_viewPort.pixelSize = 2;

    memset(grid, 0, sizeof(BLOCK_MATERIALS)*GRID_SIZE*GRID_SIZE);
    memset(next_grid, 0, sizeof(BLOCK_MATERIALS)*GRID_SIZE*GRID_SIZE);
    grid[0][0]=SAND;

    selectedMaterial = SAND;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

}

void VP::HandleInput(){
    
    // material select
    if (Input::IsKeyDown(SDL_SCANCODE_1))
    {
        selectedMaterial = SAND;
    }  
    if (Input::IsKeyDown(SDL_SCANCODE_2))
    {
        selectedMaterial = AIR;
    }
    if (Input::IsKeyDown(SDL_SCANCODE_3))
    {
        selectedMaterial = WATER;
    }
    
    // mouse
    if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT))
    {
        int mouseX;
        int mouseY;
        Input::GetMousePosition(mouseX,mouseY);
        int blockX = (mouseX - g_viewPort.x) / BLOCK_SIZE;
        int blockY = (mouseY - g_viewPort.y) / BLOCK_SIZE;
        // check if mouse is within viewport bounds
        if (blockX < 0 || blockX >= GRID_SIZE || blockY < 0 || blockY >= GRID_SIZE) {
            printf("mouse out of bounds: %d, %d\n", blockX, blockY);
            return;
        }
        next_grid[blockX][blockY] = selectedMaterial;
    }
}


bool VP::CheckPosIsMaterial(int i, int j, BLOCK_MATERIALS m){
    if (grid[i][j] == m && j<=GRID_SIZE-1 && i<=GRID_SIZE-1 && j>=0 && i>=0)    
    {
        return true;
    }
    return false;
}

bool VP::CheckPosIsSolid(int i, int j){
    if (grid[i][j] == AIR)
    {
        return false;
    }
    return true;
}

void VP::UpdateGrid(){

    for (int i = GRID_SIZE-1; i >= 0; i--)
    {
        for (int j = GRID_SIZE-1; j >=0; j--)
        {
            // sand logic
            if (grid[i][j] == SAND)
            {
                if (grid[i][j+1] == AIR && j!=GRID_SIZE-1)    
                {
                    next_grid[i][j+1] = SAND;
                    next_grid[i][j] = AIR;
                }
                else if (grid[i][j+1] == WATER && i!=GRID_SIZE-1)
                {
                    next_grid[i][j+1] = SAND;
                    next_grid[i][j] = AIR;
                }
                else if (CheckPosIsMaterial(i+1,j+1,AIR) && CheckPosIsMaterial(i-1,j+1,AIR))
                {
                    // coin flip
                    if (rand() % 2 == 0)
                    {
                        next_grid[i+1][j+1] = SAND;
                        next_grid[i][j] = AIR;
                    }
                    else
                    {
                        next_grid[i-1][j+1] = SAND;
                        next_grid[i][j] = AIR;
                    }
                }
                else if (CheckPosIsMaterial(i+1,j+1,AIR))
                {
                    next_grid[i+1][j+1] = SAND;
                    next_grid[i][j] = AIR;
                }
                else if (CheckPosIsMaterial(i-1,j+1,AIR))
                {
                    next_grid[i-1][j+1] = SAND;
                    next_grid[i][j] = AIR;
                }
            }
        }
    }

    // copy next_grid to grid
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = next_grid[i][j];
        }
    }

    for (int i = GRID_SIZE - 1; i >= 0; i--)
    {
        for (int j = GRID_SIZE - 1; j >= 0; j--)
        {
            // water logic
            if (grid[i][j] == WATER)
            {
                if (CheckPosIsMaterial(i, j + 1, AIR))
                {
                    next_grid[i][j+1] = WATER;
                    next_grid[i][j] = AIR;
                }
                else if (CheckPosIsMaterial(i + 1, j + 1, AIR))
                {
                    next_grid[i+1][j+1] = WATER;
                    next_grid[i][j] = AIR;
                }
                else if (CheckPosIsMaterial(i-1,j+1,AIR))
                {
                    next_grid[i-1][j+1] = WATER;
                    next_grid[i][j] = AIR;
                }
                else if (CheckPosIsSolid(i-1,j+1) && CheckPosIsSolid(i+1,j+1) && CheckPosIsSolid(i,j+1))
                {
                    int r = rand() % 2;
                    if (r == 0) r = -1;

                    if (!CheckPosIsSolid(i+r,j))
                    {
                        next_grid[i+r][j] = WATER;
                        next_grid[i][j] = AIR;
                    }
                }
            }
        }
    }

    // copy next_grid to grid
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = next_grid[i][j];
        }
    }
}

void VP::GetMaterialColor(BLOCK_MATERIALS m, int &r, int &g, int &b, int &a){
    switch (m)
    {
    case AIR:
        r=0;
        g=0;
        b=0;
        a=0;
        break;
    case SAND:
        r=255;
        g=255;
        b=0;
        a=255;
        break;
    case WATER:
        r=0;
        g=0;
        b=255;
        a=255;
        break;
    default:
        r=0;
        g=0;
        b=0;
        a=255;
        break;
    }
}

void VP::DrawBlock(int i, int j, BLOCK_MATERIALS m){

    int r,g,b,a;

    a = 255;

    GetMaterialColor(m, r, g, b, a);

    if (m == AIR) return;


    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {g_viewPort.x+i*BLOCK_SIZE, g_viewPort.y+j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

void VP::DrawMouseBlock(int i, int j, BLOCK_MATERIALS m){
    int r,g,b,a;
    GetMaterialColor(m, r, g, b, a);

    a = a/2; // faded

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {g_viewPort.x+i*BLOCK_SIZE, g_viewPort.y+j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

void VP::DrawGrid()
{
    int x = g_viewPort.x;
    int y = g_viewPort.y;
    int width = g_viewPort.width;
    int height = g_viewPort.height;
    int borderPixelSize = 1;
 
 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    { // outside edges
    SDL_Rect top_rect = {x, y, width, borderPixelSize};
    SDL_RenderFillRect(renderer, &top_rect);

    SDL_Rect bot_rect = {x, y+height, width, borderPixelSize};
    SDL_RenderFillRect(renderer, &bot_rect);

    SDL_Rect left_rect = {x, y, borderPixelSize, height};
    SDL_RenderFillRect(renderer, &left_rect);

    SDL_Rect right_rect = {x+width, y, borderPixelSize, height};
    SDL_RenderFillRect(renderer, &right_rect);
    } 

    // squares vertical
    for ( int i = 0; i <= width; i=i+BLOCK_SIZE)
    {
        SDL_Rect rect = {x + i, y, BLOCK_BORDER_SIZE, height};
        SDL_RenderFillRect(renderer, &rect);
    }
    // squares horizontal
    for ( int i = 0; i <= width; i=i+BLOCK_SIZE)
    {
        SDL_Rect rect = {x , y+i, width, BLOCK_BORDER_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void VP::DrawViewPort(){
    // draw white lines at every edge of the viewport
    int x = g_viewPort.x;
    int y = g_viewPort.y;
    int width = g_viewPort.width;
    int height = g_viewPort.height;
    int borderPixelSize = 1;
    
    DrawGrid();

    // run through grid, draw based on material type
    for (int i = 0 ; i< GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            DrawBlock(i,j,grid[i][j]);
        }
        
    }


    // draw mouse position preview
    int mouseX;
    int mouseY;
    Input::GetMousePosition(mouseX,mouseY);
    int blockX = (mouseX - g_viewPort.x) / BLOCK_SIZE;
    int blockY = (mouseY - g_viewPort.y) / BLOCK_SIZE;
    DrawMouseBlock(blockX,blockY,selectedMaterial);  

}