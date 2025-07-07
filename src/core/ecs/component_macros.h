#pragma once

#include "components.h"
#include "../engine.h"

// Define the xcomponent macro for component addition
#define xcomponent(ENUM_NAME, TYPE_NAME, ID, PARAMS, CALLARGS)                   \
inline void ADD_##ENUM_NAME PARAMS                              \
{                                                                                 \
    g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_##ENUM_NAME);   \
    TYPE_NAME##Component* comp = (TYPE_NAME##Component*)                          \
        g_Engine.componentArrays.GetComponentData(entity, COMPONENT_##ENUM_NAME); \
    if (comp) {                                                                  \
        comp->Init CALLARGS;                                                     \
    } else {                                                                     \
        printf("failed to retrieve %s component for entity %u\n", #TYPE_NAME, entity); \
    }                                                                            \
} 

#include "components/components.def"
#undef xcomponent 