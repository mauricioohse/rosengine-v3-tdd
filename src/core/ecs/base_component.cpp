#include "base_component.h"
#include "component_storage.h"
#include "component_manager.h"

template<typename T>
bool Component<T>::RegisterType() {
    ComponentStorage::Register<T>();
    return true;
}

template<typename T>
T* Component<T>::Get(EntityID entity) {
    return ComponentManager<T>::Get(entity);
}

template<typename T>
bool Component<T>::registered = Component<T>::RegisterType();

// Explicit template instantiations for commonly used components
#include "components.h"

// Force registration by accessing the static member
template class Component<TransformComponent>;
template class Component<SpriteComponent>;
template class Component<ColliderComponent>;
template class Component<JetAnimationComponent>;
template class Component<LifeTimeComponent>;
template class Component<TowerComponent>;
template class Component<ElementComponent>;
template class Component<EnemyComponent>;
template class Component<TextComponent>;
template class Component<UIBoxComponent>;

// Force static initialization by referencing the static members
namespace {
    void ForceComponentRegistration() {
        printf("Forcing component registration...\n");
        // Access static members to force initialization
        (void)Component<TransformComponent>::registered;
        (void)Component<SpriteComponent>::registered;
        (void)Component<ColliderComponent>::registered;
        (void)Component<JetAnimationComponent>::registered;
        (void)Component<LifeTimeComponent>::registered;
        (void)Component<TowerComponent>::registered;
        (void)Component<ElementComponent>::registered;
        (void)Component<EnemyComponent>::registered;
        (void)Component<TextComponent>::registered;
        (void)Component<UIBoxComponent>::registered;
        printf("Component registration complete.\n");
    }
    
    // Call the function during static initialization
    static bool dummy = (ForceComponentRegistration(), true);
}
