// set all to disabled for example, or enabled -> useful for toggling
void set_children_by_id_byte(ecs* world, entity e, entity id, byte value) {
    if (zox_has_id(e, id)) {
        zox_set_id(e, id, byte, value);
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            set_children_by_id_byte(world, e2, id, value);
        }
    }
}
