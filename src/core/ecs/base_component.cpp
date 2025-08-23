#include "base_component.h"
#include "component_storage.h"

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
