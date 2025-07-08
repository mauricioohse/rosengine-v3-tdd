#pragma once

// file for managing the grid functions.



/* 
    to keep same reference as SDL x and y points
    (0,0) is the top leftmost point
    X is the horizontal and increases to the right
    Y is the vertical and increases to the bottom
*/
typedef struct Point{
    int x;
    int y;

    Point operator+(Point const& obj)
    {
        return Point{x+obj.x, y+obj.y};
    }
} Point;

namespace Grid {
    extern Point GRID_START_POINT;
    extern Point GRID_END_POINT;

    void DrawGrid();
    Point GetNearestGridPoint(int x, int y); // aligned on the top left
    bool IsInsideGrid(int x, int y);
}