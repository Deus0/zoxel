byte touchscreen_is_any_input(ecs *world, entity e) {
    if (!e || !zox_valid(e)) {
        return 0;
    }
    uint children_capacity = zox_children_capacity;
    entity children[children_capacity];
    uint children_length = zox_get_children(world, e, children, children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
        if (!e2 || !zox_has(e2, ZevicePointer) || !zox_has(e2, Finger)) {
            continue;
        }
        zox_geter(e2, ZevicePointer, zevicePointer);
        if (zevice_pointer_has_input(zevicePointer)) {
            return 1;
        }
    }
    return 0;
}

byte mouse_is_any_input(ecs *world, entity e) {
    if (!e || !zox_valid(e)) {
        return 0;
    }
    uint children_capacity = zox_children_capacity;
    entity children[children_capacity];
    uint children_length = zox_get_children(world, e, children, children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
        if (!zox_has(e2, ZevicePointer)) {
            continue;
        }
        zox_geter(e2, ZevicePointer, zevicePointer);
        if (zevice_pointer_has_input(zevicePointer)) {
            return 1;
        }
    }
    return 0;
}
