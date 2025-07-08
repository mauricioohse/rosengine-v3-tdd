#include "systems.h"


struct enemy_movement_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;

}; 