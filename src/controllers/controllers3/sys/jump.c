// #define zox_log_jumping

// todo: it shouldnt use setters
//      - we should get player data from a Character!
zox_sys2(Player3DJumpSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CharacterLink, character_link);
        zox_sys_i(DeviceMode, mode);
        zox_sys_i(DeviceLinks, devices);
        entity character = character_link->value;
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }
        zox_geter(character, DisableMovement, disableMovement);
        if (disableMovement->value) {
            continue;
        }
        zox_geter_value(character, JumpState, byte, jump_state);
        if (jump_state != zox_dirty_none) {
            continue;
        }
        zox_geter_value(character, CanJump, byte, can_jump);
        if (!can_jump) { // || can_jump >= jump_cooldown_state) {
            continue;
        }
        byte is_jump_triggered = 0;
        for (int j = 0; j < devices->length; j++) {
            entity e2 = devices->value[j];
            if (!zox_valid(e2) || zox_gett_value(e2, DeviceDisabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, e2, children, children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (!zox_valid(e3)) {
                    continue;
                }
                zox_geter_value(e3, ZeviceDisabled, byte, disabled);
                if (disabled) {
                    continue;
                }
                if (zox_has(e3, ZeviceButton)) {
                    zox_geter_value(e3, DeviceButtonType, byte, type)
                    if (type == zox_device_button_a) {
                        zox_geter(e3, ZeviceButton, zeviceButton)
                        if (devices_get_pressed(zeviceButton->value)) {
                            is_jump_triggered = 1;
                        }
                    }
                }
            }
            if (mode->value == zox_device_mode_keyboardmouse && zox_has(e2, Keyboard)) {
                zox_geter(e2, Keyboard, keyboard)
                if (keyboard->space.is_pressed) {
                    is_jump_triggered = 1;
                }
            }
        }
        if (!is_jump_triggered) {
            continue;
        }
        if (!zox_gett_value(character, Jump)) {
            zox_set(character, JumpState, { zox_dirty_trigger });
#ifdef zox_log_jumping
            zox_log("+ %s jumping (%f)", zox_get_name(character), zox_current_time);
#endif
        }
    }
} zox_sys_end(Player3DJumpSystem);
