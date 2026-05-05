#define find_child_with_tag(e, tag, child_name)\
    /*zox_geter(e, Children, children_##tag);*/\
    entity children_##tag[hierarhys_children_capacity];\
    uint tag_##length = zox_get_children(world, e, children_##tag, hierarhys_children_capacity);\
    entity child_name = 0;\
    for (uint i = 0; i < tag_##length; i++) {\
        entity child_e = children_##tag[i];\
        if (child_e && zox_has(child_e, tag)) {\
            child_name = child_e;\
            break;\
        }\
    }

entity find_child_with_tag2(ecs* world, entity e, entity tag) {
    if (!zox_valid(e)) {
        return 0;
    }
    // zox_geter(e, Children, children);
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
}

entity find_child_with_tag_recursive(ecs* world, entity e, entity tag) {

    if (!zox_valid(e)) { // || !zox_has(e, Children)) {
        return 0;
    }

    entity children[hierarhys_children_capacity];
    uint children_length = zox_get_children(world, e, children, hierarhys_children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
    //zox_geter(e, Children, children);
    //for (int i = 0; i < children->length; i++) {
        //entity e2 = children->value[i];
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
}

#define find_child_with_id(e, id, child_name)\
    entity child_name = 0;\
    /*const Children *children = zox_get(parent, Children)*/\
    /*for (int i = 0; i < children->length; i++) {*/\
        /*const entity child_e = children->value[i];*/\
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
/*const Children *children_##tag = zox_get(e, Children)*/\
/*for (int i = 0; i < children_##tag->length; i++) {*/\
    /*entity child_e = children_##tag->value[i];*/\
    if (e2 && zox_has(e2, tag)) {\
        child_##tag = e2;\
        break;\
    }\
}\
if (child_##tag)

/*
#define if_has_child_with_id(e, tag, name)\
    entity name = 0;\
    zox_geter(e, Children, children_);\
    for (int ii = 0; ii < children_->length; ii++) {\
        const entity child_e = children_->value[ii];\
        if (child_e && zox_has_id(child_e, tag)) {\
            name = child_e;\
            break;\
        }\
    }\
    if (name)
*/

/*void on_child_added(ecs *world, entity parent, entity child) {
    // zox_log(" + added [%lu] to canvas [%lu]\n", e, canvas)
    // todo: make this generic for when component is set, event
    // this isn't systematic enough for children linking!
    Children *children = zox_get_mut(parent, Children);
    if (add_to_Children(children, child)) {
        zox_modified(parent, Children);
    }
}*/

void zox_debug_children(ecs *world, entity parent) {
    Children *children = zox_get_mut(parent, Children);
    zox_log(" > children of [%s]\n", zox_get_name(parent));
    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];
        zox_log("       - child [%s]\n", zox_get_name(child));
    }
}
