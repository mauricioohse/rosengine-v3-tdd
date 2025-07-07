#include "../../core/ecs/systems.h"


struct game_name_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;

    bool is_visible = 1;
    EntityID titleEntity = INVALID_ENTITY;

}; 