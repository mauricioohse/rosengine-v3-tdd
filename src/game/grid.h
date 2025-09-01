#pragma once
#include "SDL.h"

// file for managing the grid functions.



/*   to keep same reference as SDL x and y points
  (0,0) is the top leftmost point
  X is the horizontal and increases to the right
  Y is the vertical and increases to the bottom
*/
struct Point{
    int x;
    int y;

    Point operator+(Point const& obj) const
    {
        return Point{x+obj.x, y+obj.y};
    }
};

namespace Grid {
    extern Point GRID_START_POINT;
    extern Point GRID_END_POINT;
    extern int GRID_SQUARE_LENGTH;
    extern int GRID_HORIZONTAL_SQUARE_COUNT;
    extern int GRID_VERTICAL_SQUARE_COUNT;

    void DrawGrid(bool isDebug = false);
    Point GetNearestGridPoint(int x, int y);            // aligned on the top left
    Point GetNearestGridPointCenter(int x, int y);      // center aligned
    Point GetNearestDualGridPoint(int x, int y);        // aligned to grid offset
    Point GetNearestDualGridPointCenter(int x, int y);  // center aligned
    bool IsInsideGrid(int x, int y);
    bool LoadLevel(const char * filename);
    Point GridToScreenPosition(int grid_x, int grid_y);
    int CoordToIndex(int x, int y);
    
    // monster path access functions
    Point GetMonsterPathPoint(int index);
    int GetMonsterPathSize();

    // autotile
    // 16 possible combinations
    enum TileShape {
        SHAPE_GRASS = 0,        // 0000 grass sprite id
        SHAPE_DIRT,             // 1111 dirt sprite id
        SHAPE_INNER_CORNER_TL,  // 1000 grass with dirt on top left outer corner dirt
        SHAPE_INNER_CORNER_TR,  // 0100 grass with dirt on top right outer corner dirt
        SHAPE_INNER_CORNER_BR,  // 0010 grass with dirt on bottom right outer corner dirt
        SHAPE_INNER_CORNER_BL,  // 0001 grass with dirt on bottom left outer corner dirt
        SHAPE_OUTER_CORNER_BR,  // 0111 dirt with grass on bottom right outer corner dirt    
        SHAPE_OUTER_CORNER_BL,  // 1011 dirt with grass on bottom left outer corner dirt
        SHAPE_OUTER_CORNER_TL,  // 1101 dirt with grass on top left outer corner dirt
        SHAPE_OUTER_CORNER_TR,  // 1110 dirt with grass on top right outer corner dirt
        SHAPE_EDGE_TOP,         // 1100 grass with dirt on top
        SHAPE_EDGE_RIGHT,       // 0110 grass with dirt on right
        SHAPE_EDGE_BOTTOM,      // 0011 grass with dirt on bottom
        SHAPE_EDGE_LEFT,        // 1001 grass with dirt on left
        SHAPE_DIAG_TL_BR,       // 1010 
        SHAPE_DIAG_TR_BL,       // 0101 
        SHAPE_MISC,             // fallback
        SHAPE_COUNT
    };

    struct TileSpriteInfo {
        TileShape shape;
        int spriteId;
    };

    // sprites
    bool LoadTileSprites(SDL_Renderer* renderer);
    void FreeTileSprites();
    void InitTileLookup();
    void DrawAutoTiledMap(int map[100][100], int maxX, int maxY);
}