// NOTE: This uses flecs events to handle children:
//      - destruction (OnRemove)
//      - adding (OnSet)
//  However OldParentLink + custom (ReparentSystem) was needed for removing links

#define zoxc_child(T, parent_T)\
zoxc_entity(T);\
\
void on_remove_##T(iter *it) {\
    zox_sys_world();\
    zox_sys_begin();\
    zox_sys_in(T);\
    for (int i = 0; i < it->count; i++) {\
        zox_sys_e();\
        zox_sys_i(T, parent);\
        if (!zox_valid(parent->value) || !zox_has(parent->value, parent_T)) {\
            continue;\
        }\
        zox_mut_begin(parent->value, parent_T, links);\
        if (remove_from_##parent_T(links, e)) {\
            zox_modified(parent->value, parent_T);\
        }\
    }\
}\
\
void on_set_##T(iter *it) {\
    zox_sys_world();\
    zox_sys_begin();\
    zox_sys_in(T);\
    for (int i = 0; i < it->count; i++) {\
        zox_sys_e();\
        zox_sys_i(T, parent);\
        if (!zox_valid(parent->value) || !zox_has(parent->value, parent_T)) {\
            continue;\
        }\
        zox_mut_begin(parent->value, parent_T, links);\
        if (!is_in_##parent_T(links, e) && add_to_##parent_T(links, e)) {\
            zox_modified(parent->value, parent_T);\
        }\
    }\
}

#define zoxd_child(T)\
    zoxd_entity(T);\
    zox_observe_expr(on_remove_##T, EcsOnRemove, "[in] "#T);\
    zox_observe_expr(on_set_##T, EcsOnSet, "[in] "#T)
