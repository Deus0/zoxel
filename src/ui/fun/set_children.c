// set all to disabled for example, or enabled -> useful for toggling
void set_children_component_byte(
    ecs* world,
    entity e,
    entity id,
    const byte value
) {
    if (zox_has_id(e, id)) {
        zox_set_id(e, id, 1, value);
    }
    /*
    if (zox_has(e, Children)) {
        zox_geter(e, Children, children)
        for (int i = 0; i < children->length; i++) {*/

    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        set_children_component_byte(
            world,
            child,
            id,
            value);
    }
}