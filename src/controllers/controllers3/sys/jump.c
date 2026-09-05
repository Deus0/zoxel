// #define zox_log_jumping

// todo: it shouldnt use setters
//      - we should get player data from a Character!
zox_sys2(Player3DJumpSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(DeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(DeviceMode, mode);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        entity character = zox_get_link(world, e, Character);
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }
        if (zox_has(character, DisableMovement)) {
            continue;
        }
        /*byte jump_state = zox_getv(character, JumpState);
        if (jump_state) {
            continue;
        }*/
        if (!zox_getv(character, CanJump)) { // || can_jump >= jump_cooldown_state) {
            continue;
        }
        byte is_jump_triggered = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            //for (int j = 0; j < devices->length; j++) {
            //entity e2 = devices->value[j];
            if (!zox_valid(e2) || zox_getv(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (zox_getv(e3, ZeviceDisabled)) {
                    continue;
                }
                if (zox_has(e3, ZeviceButton)) {
                    byte type = zox_getv(e3, DeviceButtonType);
                    byte clicked = zox_getv(e3, ZeviceButton);
                    if (type == zox_btn_a) {
                        if (devices_get_pressed(clicked)) {
                            is_jump_triggered = 1;
                        }
                    }
                }
            }
            if (mode->value == zox_device_mode_keyboardmouse && zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard);
                if (keyboard->space.is_pressed) {
                    is_jump_triggered = 1;
                }
            }
        }
        if (!is_jump_triggered) {
            continue;
        }
        // if (!zox_getv(character, Jump)) {
        zox_setv(character, JumpState, jump_state_trigger);
#ifdef zox_log_jumping
        zox_log("Triggered %s jumping", zox_getn(character));
#endif
    }
} zox_sys_end(Player3DJumpSystem);
