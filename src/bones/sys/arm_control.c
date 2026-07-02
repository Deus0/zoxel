// NOTE: Tells the Limbs to swing or not
// TODO: Only signal the bones when WalkStateDirty
zox_sys2(ArmControlSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WalkState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WalkState, state);
        entity bones[zox_children_capacity];
        uint length = zox_get_children_by_id_recursive(world, e, bones, zox_children_capacity, zox_id(SwingState), 0);
        for (uint j = 0; j < length; j++) {
            entity bone = bones[j];
            zox_muter(bone, SwingState, swing);
            if (swing->value) {
                // NOTE: 1 is idle and 2 is moving
                swing->value = state->value + 1;
            }
        }
    }
} zox_sys_end(ArmControlSystem);
