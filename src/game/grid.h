#pragma once

// file for managing the grid functions.



/* 
    to keep same reference as SDL x and y points
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

    void DrawGrid();
    Point GetNearestGridPoint(int x, int y); // aligned on the top left
    Point GetNearestGridPointCenter(int x, int y); // center aligned
    bool IsInsideGrid(int x, int y);
    bool LoadLevel(const char * filename);
    Point GridToScreenPosition(int grid_x, int grid_y);
    int CoordToIndex(int x, int y);
    
    // monster path access functions
    Point GetMonsterPathPoint(int index);
    int GetMonsterPathSize();
}