#include "core/engine.h"
#include "game/game.h"

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[]) {
    //TestEntityManager();
    
    if (!Engine::Init()) {
        printf("Engine initialization failed!\n");
        return -1;
    }
    
    if (!g_Game.Init()) {
        printf("Game initialization failed!\n");
        return -1;
    }
    
    g_Engine.Run();
    
    g_Game.Cleanup();
    Engine::Cleanup();
    return 0;
}
