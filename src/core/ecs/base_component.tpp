#include "component_storage.h"
#include "component_manager.h"

template<typename T>
bool Component<T>::registered = false;

template<typename T>
bool Component<T>::RegisterType() {
    if (!registered) {
        ComponentStorage::Register<T>();
        registered = true;
    }
    return true;
}

template<typename T>
T* Component<T>::Get(EntityID entity) {
    return ComponentManager<T>::Get(entity);
}
