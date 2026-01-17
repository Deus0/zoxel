#define find_child_with_tag(parent, tag, child_name)\
    zox_geter(parent, Children, children_##tag);\
    entity child_name = 0;\
    for (int i = 0; i < children_##tag->length; i++) {\
        entity child_e = children_##tag->value[i];\
        if (child_e && zox_has(child_e, tag)) {\
            child_name = child_e;\
            break;\
        }\
    }

entity find_child_with_tag_recursive(ecs* world, entity e, entity tag) {

    if (!zox_valid(e) || !zox_has(e, Children)) {
        return 0;
    }

    zox_geter(e, Children, children);
    for (int i = 0; i < children->length; i++) {
        entity e2 = children->value[i];

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

#define find_child_with_id(parent, id, child_name)\
    const Children *children = zox_get(parent, Children)\
    entity child_name = 0;\
    for (int i = 0; i < children->length; i++) {\
        const entity child_e = children->value[i];\
        if (child_e && zox_has_id(child_e, id)) {\
            child_name = child_e;\
            break;\
        }\
    }

#define if_has_child_with_tag(e, tag)\
entity child_##tag = 0;\
const Children *children_##tag = zox_get(e, Children)\
for (int i = 0; i < children_##tag->length; i++) {\
    const entity child_e = children_##tag->value[i];\
    if (child_e && zox_has(child_e, tag)) {\
        child_##tag = child_e;\
        break;\
    }\
}\
if (child_##tag)


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

void on_child_added(ecs *world, entity parent, entity child) {
    // zox_log(" + added [%lu] to canvas [%lu]\n", e, canvas)
    // todo: make this generic for when component is set, event
    // this isn't systematic enough for children linking!
    Children *children = zox_get_mut(parent, Children);
    if (add_to_Children(children, child)) {
        zox_modified(parent, Children);
    }
}

void zox_debug_children(ecs *world, entity parent) {
    Children *children = zox_get_mut(parent, Children);
    zox_log(" > children of [%s]\n", zox_get_name(parent));
    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];
        zox_log("       - child [%s]\n", zox_get_name(child));
    }
}
