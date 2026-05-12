/*#define find_child_with_tag(e, tag, child_name)\
    entity children_##tag[hierarhys_children_capacity];\
    uint tag_##length = zox_get_children(world, e, children_##tag, hierarhys_children_capacity);\
    entity child_name = 0;\
    for (uint i = 0; i < tag_##length; i++) {\
        entity child_e = children_##tag[i];\
        if (child_e && zox_has(child_e, tag)) {\
            child_name = child_e;\
            break;\
        }\
    }*/

/*entity find_child_with_tag2(ecs* world, entity e, entity tag) {
    if (!zox_valid(e)) {
        return 0;
    }
    entity children[hierarhys_children_capacity];
    uint children_length = zox_get_children(world, e, children, hierarhys_children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
        if (!zox_valid(e2)) {
            continue;
        }
        if (zox_has_id(e2, tag)) {
            return e2;
        }
    }
    return 0;
}*/

/*entity find_child_with_tag_recursive(ecs* world, entity e, entity tag) {
    if (!zox_valid(e)) {
        return 0;
    }
    entity children[hierarhys_children_capacity];
    uint children_length = zox_get_children(world, e, children, hierarhys_children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
        if (!zox_valid(e2)) {
            continue;
        }
        if (zox_has_id(e2, tag)) {
            return e2;
        }
        entity e3 = find_child_with_tag_recursive(world, e2, tag);
        if (e3) {
            return e3;
        }
    }
    return 0;
}*/

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
