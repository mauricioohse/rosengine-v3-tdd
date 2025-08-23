#include "systems.h"


struct enemy_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities) override;
    void Destroy() override;

}; 