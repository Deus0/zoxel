

zox_sys2(ParentPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_in(LocalPosition3D);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ParentLink, parent);
        zox_sys_i(LocalPosition3D, local);
        zox_sys_o(Position3D, position);

        if (!zox_valid(parent->value)) {
            continue;
        }

        set_position_from_parents(world, parent->value, &position->value, local->value);
    }
} zox_sys_end(ParentPositionSystem);
