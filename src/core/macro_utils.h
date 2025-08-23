#pragma once

#ifndef MACRO_UTILS_H
#define MACRO_UTILS_H
// do function only once macro
#define DO_ONCE(code) \
    do { \
        static int done = 0; \
        if (!done) { \
            done = 1; \
            code; \
        } \
    } while(0)

// #define ycomponent(ENUM_NAME, TYPE_NAME, ID, ...)

// /**
//  * @brief Iterates over all entities in a scene that have a specific component.
//  * 
//  * This macro simplifies querying for entities with a given component. It handles the
//  * loop, the check, and the component data retrieval.
//  *
//  * @param p_scene       A pointer to the SceneBase object.
//  * @param p_entity_var  The name of the variable to hold the current EntityID.
//  * @param p_comp_type   The C type of the component struct (e.g., TargetComponent).
//  * @param p_comp_enum   The enum value for the component (e.g., C_Target.
//  * @param p_comp_var    The name of the variable to hold the pointer to the component data.
//  */
// #define FOR_EACH_COMPONENT(p_scene, p_entity_var, p_comp_type, p_comp_var) \
//     for (EntityID p_entity_var : p_scene->entities) \
//     { \
//         p_comp_type##Component* p_comp_var = (p_comp_type##Component*)GET_COMPONENT(p_entity_var, C_##p_comp_type); \
//         if (p_comp_var) \
//         {

// #define FOR_EACH_COMPONENT_2(p_scene, p_entity_var, p_comp_type_1, p_comp_var_1, p_comp_type_2, p_comp_var_2)                                            \
//     for (EntityID p_entity_var : p_scene->entities)                                                                                                      \
//     {                                                                                                                                                    \
//         p_comp_type_1##Component *p_comp_var_1 = (p_comp_type_1##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_1); \
//         p_comp_type_2##Component *p_comp_var_2 = (p_comp_type_2##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_2); \
//         if (p_comp_var_1 && p_comp_var_2)                                                                                                                \
//         {

// #define FOR_EACH_COMPONENT_3(p_scene, p_entity_var, p_comp_type_1, p_comp_var_1, p_comp_type_2, p_comp_var_2, p_comp_type_3, p_comp_var_3)                                            \
//     for (EntityID p_entity_var : p_scene->entities)                                                                                                      \
//     {                                                                                                                                                    \
//         p_comp_type_1##Component *p_comp_var_1 = (p_comp_type_1##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_1); \
//         p_comp_type_2##Component *p_comp_var_2 = (p_comp_type_2##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_2); \
//         p_comp_type_3##Component *p_comp_var_3 = (p_comp_type_3##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_3); \
//         if (p_comp_var_1 && p_comp_var_2 && p_comp_var_3)                                                                                                                \
//         {

// #define FOR_EACH_COMPONENT_4(p_scene, p_entity_var, p_comp_type_1, p_comp_var_1, p_comp_type_2, p_comp_var_2, p_comp_type_3, p_comp_var_3, p_comp_type_4, p_comp_var_4 )               \
//     for (EntityID p_entity_var : p_scene->entities)                                                                                                      \
//     {                                                                                                                                                    \
//         p_comp_type_1##Component *p_comp_var_1 = (p_comp_type_1##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_1); \
//         p_comp_type_2##Component *p_comp_var_2 = (p_comp_type_2##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_2); \
//         p_comp_type_3##Component *p_comp_var_3 = (p_comp_type_3##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_3); \
//         p_comp_type_4##Component *p_comp_var_4 = (p_comp_type_4##Component *)GET_COMPONENT(p_entity_var, C_##p_comp_type_4); \
//         if (p_comp_var_1 && p_comp_var_2 && p_comp_var_3 && p_comp_var_4)                                                                                                \
//         {

// // Note: The FOR_EACH_COMPONENT_X macros intentionally leaves the block open.
// // You close it with two '}' in your system function.
// #define ); }}


#endif // MACRO_UTILS_H
