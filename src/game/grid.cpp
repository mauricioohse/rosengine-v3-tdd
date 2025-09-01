#include "grid.h"
#include "engine_constants.h"
#include "engine.h"
#include "window.h"
#include "input.h"
#include "stdio.h"
#include "string.h"
#include "SDL_image.h"

#define DEBUG_DRAW_HOVERED_SQUARE 1

using namespace Grid;

// constants - change here to resize the grid and the squares
static int GRID_MAX_WIDTH = (int) (WINDOW_WIDTH*0.80); // in fraction of screen
static int GRID_MAX_HEIGHT = (int) (WINDOW_HEIGHT*0.80);
int Grid::GRID_SQUARE_LENGTH = 48;
int Grid::GRID_HORIZONTAL_SQUARE_COUNT =  (int) GRID_MAX_WIDTH/Grid::GRID_SQUARE_LENGTH;
int Grid::GRID_VERTICAL_SQUARE_COUNT = (int)  GRID_MAX_HEIGHT/Grid::GRID_SQUARE_LENGTH;
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

static Texture* g_TileSpritesheet = NULL;

static Point monster_path[100] = {0,0};
static int monster_path_size = 0;

Point Grid::GRID_START_POINT = {20,20};
Point Grid::GRID_END_POINT = {
    GRID_START_POINT.x + GRID_HORIZONTAL_SQUARE_COUNT * GRID_SQUARE_LENGTH,
    GRID_START_POINT.y + GRID_VERTICAL_SQUARE_COUNT *GRID_SQUARE_LENGTH,
};

void Grid::DrawGrid(bool isDebug){
    bool drawGridMain = false;
    bool drawGridDual = false;
    bool drawMonsterPath = false;

    if (isDebug) {
        drawGridMain = true;
        drawGridDual = false;
        drawMonsterPath = true;
    }
    
    Grid::InitTileLookup();
    // draw the auto-tiled map first
    if (currLevel.isLoaded && g_TileSpritesheet != NULL) {
        Grid::DrawAutoTiledMap(currLevel.LoadedGrid, currLevel.maxX, currLevel.maxY);
    }

    if (drawGridMain) {
        // set render draw to white
        SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 255, 255, 255);

        // vertical lines
        for (int i = 0; i <= GRID_HORIZONTAL_SQUARE_COUNT; i++){
            int x1 = GRID_START_POINT.x + i * GRID_SQUARE_LENGTH;
            int y1 = GRID_START_POINT.y;
            int x2 = x1;
            int y2 = y1 + GRID_CLAPMED_HEIGHT;
            SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x2, y2);
        }

        // horizontal lines
        for (int j = 0; j <= GRID_VERTICAL_SQUARE_COUNT; j++){
            int x1 = GRID_START_POINT.x;
            int y1 = GRID_START_POINT.y + j * GRID_SQUARE_LENGTH;
            int x2 = x1 + GRID_CLAMPED_WIDTH;
            int y2 = y1;
            SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x2, y2);
        }
    }
        
    if (drawGridDual) {
            int offset = GRID_SQUARE_LENGTH / 2;
            SDL_SetRenderDrawColor(g_Engine.window->renderer, 180, 180, 180, 90);

            for (int i = 0; i <= GRID_HORIZONTAL_SQUARE_COUNT; i++) {
                int x1 = GRID_START_POINT.x + offset + i * GRID_SQUARE_LENGTH;
                int y1 = GRID_START_POINT.y + offset;
                int y2 = y1 + GRID_CLAPMED_HEIGHT;
                SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x1, y2);
            }
            for (int j = 0; j <= GRID_VERTICAL_SQUARE_COUNT; j++) {
                int x1 = GRID_START_POINT.x + offset;
                int y1 = GRID_START_POINT.y + offset + j * GRID_SQUARE_LENGTH;
                int x2 = x1 + GRID_CLAMPED_WIDTH;
                SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x2, y1);
            }
    }
        
    // highlight mouse
    if (DEBUG_DRAW_HOVERED_SQUARE){
        int x,y;
        Input::GetMousePosition(x, y);

        if (IsInsideGrid(x,y)){
            SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 0, 0, 55);
            Point p = GetNearestGridPoint(x,y);
            SDL_Rect rect = {p.x,p.y,GRID_SQUARE_LENGTH,GRID_SQUARE_LENGTH};
            SDL_RenderDrawRect(g_Engine.window->renderer,  &rect);
        }
    }
    
    if(drawMonsterPath) {
        // paint the path that monsters walk on - it starts at number 1 until the maximum number.
        SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 0, 0, 50); // red path line
        for (int i = 0; i < monster_path_size - 1; i++) {
            SDL_RenderDrawLine(g_Engine.window->renderer, 
                            monster_path[i].x, monster_path[i].y,
                            monster_path[i+1].x, monster_path[i+1].y);
        }
        
        // draw path points
        SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 255, 0, 255); // yellow path points
        for (int i = 0; i < monster_path_size; i++) {
            SDL_Rect point_rect = {monster_path[i].x - 2, monster_path[i].y - 2, 4, 4};
            SDL_RenderFillRect(g_Engine.window->renderer, &point_rect);
        }
    }
}

static Grid::TileSpriteInfo tileLookup[Grid::SHAPE_COUNT];

void Grid::InitTileLookup() {
    // initialize lookup: each SHAPE relates to a spriteId
    tileLookup[SHAPE_GRASS]           = { SHAPE_GRASS,           0 };   // grass sprite id
    tileLookup[SHAPE_DIRT]            = { SHAPE_DIRT,            1 };   // dirt sprite id
    tileLookup[SHAPE_OUTER_CORNER_TL] = { SHAPE_INNER_CORNER_TL, 2 };   // grass with dirt on top left outer corner dirt
    tileLookup[SHAPE_OUTER_CORNER_TR] = { SHAPE_INNER_CORNER_TR, 3 };   // grass with dirt on top right outer corner dirt
    tileLookup[SHAPE_OUTER_CORNER_BR] = { SHAPE_INNER_CORNER_BR, 4 };   // grass with dirt on bottom right outer corner dirt
    tileLookup[SHAPE_OUTER_CORNER_BL] = { SHAPE_INNER_CORNER_BL, 5 };   // grass with dirt on bottom left outer corner dirt
    tileLookup[SHAPE_INNER_CORNER_TL] = { SHAPE_OUTER_CORNER_BR, 6 };   // dirt with grass on bottom right outer corner dirt    
    tileLookup[SHAPE_INNER_CORNER_TR] = { SHAPE_OUTER_CORNER_BL, 7 };   // dirt with grass on bottom left outer corner dirt
    tileLookup[SHAPE_INNER_CORNER_BR] = { SHAPE_OUTER_CORNER_TL, 8 };   // dirt with grass on top left outer corner dirt
    tileLookup[SHAPE_INNER_CORNER_BL] = { SHAPE_OUTER_CORNER_TR, 9 };   // dirt with grass on top right outer corner dirt
    tileLookup[SHAPE_EDGE_TOP]        = { SHAPE_EDGE_TOP,        10 };  // grass with dirt on top
    tileLookup[SHAPE_EDGE_RIGHT]      = { SHAPE_EDGE_RIGHT,      11 };  // grass with dirt on right
    tileLookup[SHAPE_EDGE_BOTTOM]     = { SHAPE_EDGE_BOTTOM,     12 };  // grass with dirt on bottom
    tileLookup[SHAPE_EDGE_LEFT]       = { SHAPE_EDGE_LEFT,       13 };  // grass with dirt on left
    tileLookup[SHAPE_DIAG_TL_BR]      = { SHAPE_DIAG_TL_BR,      14 };
    tileLookup[SHAPE_DIAG_TR_BL]      = { SHAPE_DIAG_TR_BL,      15 };
    tileLookup[SHAPE_MISC]            = { SHAPE_MISC,            16 };

    // if (!Grid::LoadTileSprites(g_Engine.window->renderer)) {
    //     printf("Failed to load tile sprites in InitTileLookup!\n");
    // }
    g_TileSpritesheet = ResourceManager::GetTexture(TILESHEET);
}

static Grid::TileShape MaskToShape(int mask) {
    using namespace Grid;
    switch(mask) {
        case 0b0000: return SHAPE_GRASS;
        case 0b1111: return SHAPE_DIRT;
        case 0b1000: return SHAPE_OUTER_CORNER_TL;
        case 0b0100: return SHAPE_OUTER_CORNER_TR;
        case 0b0010: return SHAPE_OUTER_CORNER_BR;
        case 0b0001: return SHAPE_OUTER_CORNER_BL;
        case 0b0111: return SHAPE_INNER_CORNER_BR;
        case 0b1011: return SHAPE_INNER_CORNER_BL;
        case 0b1101: return SHAPE_INNER_CORNER_TL;
        case 0b1110: return SHAPE_INNER_CORNER_TR;
        case 0b1100: return SHAPE_EDGE_TOP;
        case 0b0110: return SHAPE_EDGE_RIGHT;
        case 0b0011: return SHAPE_EDGE_BOTTOM;
        case 0b1001: return SHAPE_EDGE_LEFT;
        case 0b1010: return SHAPE_DIAG_TL_BR;
        case 0b0101: return SHAPE_DIAG_TR_BL;
        default:     return SHAPE_MISC;
    }
}

void Grid::DrawAutoTiledMap(int map[100][100], int maxX, int maxY) {
    if (g_TileSpritesheet == NULL) {
        printf("Tilesheet texture is not loaded. Cannot draw tiles.\n");
        return;
    }

    for (int row = 0; row < maxY - 1; row++) {
        for (int col = 0; col < maxX - 1; col++) {
            // display destination
            SDL_Rect destRect = {
                GRID_START_POINT.x + col * GRID_SQUARE_LENGTH,
                GRID_START_POINT.y + row * GRID_SQUARE_LENGTH,
                GRID_SQUARE_LENGTH,
                GRID_SQUARE_LENGTH
            };

            // define corners to build mask
            int tl = (map[row][col] > 0);     
            int tr = (map[row][col+1] > 0);   
            int br = (map[row+1][col+1] > 0); 
            int bl = (map[row+1][col] > 0);   

            // build mask and find tile shape
            int mask = (tl << 3) | (tr << 2) | (br << 1) | bl;
            TileShape shape = MaskToShape(mask);

            // find sprite ID using the tile shape
            int spriteIdToUse = tileLookup[shape].spriteId;
            
            // define the origin in the sprite sheet
            SDL_Rect srcRect = {
                spriteIdToUse * GRID_SQUARE_LENGTH,
                0,
                GRID_SQUARE_LENGTH,
                GRID_SQUARE_LENGTH
            };
            
            // draw sprite
            SDL_RenderCopy(g_Engine.window->renderer, g_TileSpritesheet->sdlTexture, &srcRect, &destRect);
        }
    }
}

bool Grid::IsInsideGrid(int x, int y){
    return (x > GRID_START_POINT.x && 
            x < GRID_END_POINT.x && 
            y > GRID_START_POINT.y && 
            y < GRID_END_POINT.y);
}

static bool CreateMonsterPath(){
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
        monster_path[monster_path_size].x = screen_pos.x;
        monster_path[monster_path_size].y = screen_pos.y;
        monster_path_size++;
        
        last_value = current_value;
        
        // find next value
        current_value++;
        bool found_next = false;
        
        // check adjacent cells for next value
        int directions[8][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}, {-1,-1}, {-1,+1}, {+1,-1}, {+1,+1}};
        for (int i = 0; i < 8; i++) {
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

bool Grid::LoadLevel(const char *filename){
    char path[100];
    path[0] = '\0';
    strncat(path, "assets/levels/", sizeof(path) - 1);
    strncat(path, filename, sizeof(path) - strlen(path) - 1);
    memset(currLevel.LoadedGrid, 0, sizeof(currLevel.LoadedGrid));

    FILE * fp = fopen(path, "r");

    memset(&currLevel, 0, sizeof(currLevel));

    if (fp){
        char line[256];
        int row = 0;
        
        while (fgets(line, sizeof(line), fp) && row < GRID_VERTICAL_SQUARE_COUNT){
            int col = 0;
            char *token = line;
            char *end;
            
            while (*token && col < GRID_HORIZONTAL_SQUARE_COUNT){
                // skip whitespace
                while (*token == ' ' || *token == '\t') token++;
                
                if (*token == '\0' || *token == '\n') break;
                
                int value = (int)strtol(token, &end, 10);
                
                if (end == token){
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

        if(!CreateMonsterPath()){
            return false;
        }

        currLevel.isLoaded = true;

        return true;
    } 
    else{
        printf("Couldnt open %s: Fail to load csv level\n", path);
    }
    

    return false;
}

// transforms any XY coord to the data index
int Grid::CoordToIndex(int x, int y){
    int grid_x = (x - GRID_START_POINT.x) / GRID_SQUARE_LENGTH;
    int grid_y = (y - GRID_START_POINT.y) / GRID_SQUARE_LENGTH;
    return grid_x + grid_y*GRID_HORIZONTAL_SQUARE_COUNT;
}

// aligned on the top left of the square that contains x and y
Point Grid::GetNearestGridPoint(int x, int y){
    // clamps x and y by GRID_SQUARE_LENGTH
    int x_count_offset = (x-GRID_START_POINT.x)/GRID_SQUARE_LENGTH;
    int y_count_offset = (y-GRID_START_POINT.y)/GRID_SQUARE_LENGTH;
    int x_pos = GRID_START_POINT.x + x_count_offset*GRID_SQUARE_LENGTH;
    int y_pos = GRID_START_POINT.y + y_count_offset*GRID_SQUARE_LENGTH;
    return Point{x_pos, y_pos};
}

Point Grid::GridToScreenPosition(int grid_x, int grid_y){
    return Point{
        GRID_START_POINT.x + grid_x * GRID_SQUARE_LENGTH,
        GRID_START_POINT.y + grid_y * GRID_SQUARE_LENGTH
    };
}

Point Grid::GetNearestGridPointCenter(int x, int y){
    Point corner = GetNearestGridPoint(x,y);
    Point center = corner + Point{Grid::GRID_SQUARE_LENGTH/2, Grid::GRID_SQUARE_LENGTH/2};
    return center;
}

Point Grid::GetMonsterPathPoint(int index){
    if (index >= 0 && index < monster_path_size) {
        return monster_path[index];
    }
    return Point{0, 0}; // return default if index out of bounds
}

int Grid::GetMonsterPathSize(){
    return monster_path_size;
}