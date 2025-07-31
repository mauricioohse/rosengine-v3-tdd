#pragma once

#include "components.h"
#include "../engine.h"

// Define the xcomponent macro for component addition
#define xcomponent(TYPE_NAME, ID, PARAMS, CALLARGS)                   \
inline void ADD_##TYPE_NAME PARAMS                              \
{                                                                                 \
    g_Engine.entityManager.AddComponentToEntity(entity, C_##TYPE_NAME);   \
    TYPE_NAME##Component* comp = (TYPE_NAME##Component*)                          \
        g_Engine.componentArrays.GetComponentData(entity, C_##TYPE_NAME); \
    if (comp) {                                                                  \
        comp->Init CALLARGS;                                                     \
    } else {                                                                     \
        printf("failed to retrieve %s component for entity %u\n", #TYPE_NAME, entity); \
    }                                                                            \
} \
\
\
inline TYPE_NAME##Component* GET_##TYPE_NAME(EntityID entity) \
{       \
    return (TYPE_NAME##Component*)  g_Engine.componentArrays.GetComponentData(entity, C_##TYPE_NAME);   \
}       \
\

#define HAS_COMPONENT( entity, COMPONENTS_MASK)     (g_Engine.entityManager.HasComponent(entity, COMPONENTS_MASK))


#include "components/components.def"
#undef xcomponent 
