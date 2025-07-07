#ifndef SANDFALL_H
#define SANDFALL_H

#include "../core/engine.h"


struct viewPort{
    int x, y, width, height;
    int pixelSize; // both width and height of the pixel
};

namespace VP
{

    enum BLOCK_MATERIALS
    {
        AIR,
        SAND,
        WATER
    };


    void InitViewPort();
    void UpdateGrid();
    void DrawViewPort();
    void DrawBlock(int i, int j, BLOCK_MATERIALS m);
    void HandleInput();
    void GetMaterialColor(BLOCK_MATERIALS m, int &r, int &g, int &b, int &a);
    void DrawMouseBlock(int i, int j, BLOCK_MATERIALS m);
    bool CheckPosIsMaterial(int i, int j, BLOCK_MATERIALS m);
    bool CheckPosIsSolid(int i, int j);
    void DrawGrid();
}

extern viewPort g_viewPort;




#endif // SANDFALL_H