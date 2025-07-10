#include "systems.h"

struct projectile_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;

    void CreateProjectile();
}; 