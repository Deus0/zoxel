#define find_child_with_id(e, id, child_name)\
    entity child_name = 0;\
    entity children[hierarhys_children_capacity];\
    uint children_length = zox_get_children(world, e, children, hierarhys_children_capacity);\
    for (uint i = 0; i < children_length; i++) {\
        entity e2 = children[i];\
        if (e2 && zox_has_id(e2, id)) {\
            child_name = e2;\
            break;\
        }\
    }

#define if_has_child_with_tag(e, tag)\
    entity child_##tag = 0;\
    entity children_##tag[hierarhys_children_capacity];\
    uint children_length = zox_get_children(world, e, children_##tag, hierarhys_children_capacity);\
    for (uint i = 0; i < children_length; i++) {\
        entity e2 = children_##tag[i];\
        if (e2 && zox_has(e2, tag)) {\
            child_##tag = e2;\
            break;\
        }\
    }\
    if (child_##tag)
