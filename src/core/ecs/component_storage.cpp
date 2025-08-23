#include "component_storage.h"

// Define the static member
std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> ComponentStorage::customArrayNameSoNoDuplicateExists;
