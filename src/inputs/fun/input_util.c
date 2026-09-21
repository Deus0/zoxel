// #define zox_debug_zevice_states

// todo: check if this is added back
extern void raycaster_select_element(ecs*, const entity, const entity);

// this disables any buttons (zevices) of a device, until they are released, then they get auto re enabled
void disable_inputs_until_release(
    ecs *world,
    entity player,
    byte device_mode)
{
    if (!zox_valid(player)) {
        return;
    }
    if (device_mode != zox_device_mode_gamepad) {
        // zox_log_error("Need to reenable this function and refactor it.")
        raycaster_select_element(
            world,
            player,
            0);
    }
    entity devices[zox_children_capacity];
    uint length = zox_get_children_by_id(
        world,
        player,
        devices,
        zox_children_capacity,
        zox_id(Device));
    for (uint j = 0; j < length; j++) {
        entity e2 = devices[j];
        if (!zox_valid(e2)) {
            continue;
        }
        if (!zox_has(e2, Gamepad)) {
            continue;
        }
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(
            world,
            e2,
            children,
            children_capacity);
        for (uint i = 0; i < children_length; i++) {
            entity e3 = children[i];
            if (zox_has(e3, Disabled)) {
                continue;
            }
            byte has_input = 0;
            if (zox_has(e3, ZeviceStick)) {
                zox_geter(e3, ZeviceStick, stick);
                has_input = zevice_stick_has_input(
                    stick,
                    joystick_min_cutoff);
            } else if (zox_has(e3, ZeviceButton)) {
                zox_geter(e3, ZeviceButton, button);
                has_input = button->value !=  0;
            }
            if (has_input) {
                zox_add(e3, Disabled);
                zox_logv("  = button disabled [%lu] at %f",
                    e3,
                    zox_current_time);
            }
        }
    }
}
