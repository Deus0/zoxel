static void dropdown_pointer_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity pointer = it->entities[i];
        const entity device = zox_get_link(world, pointer, DeviceLink);
        const entity player = zox_get_parent(world, device);
        const entity canvas = zox_get_link(world, player, CanvasLink);
        const entity dropdown = zox_get_link(world, canvas, OpenDropdown);
        if (!zox_valid(dropdown)) {
            continue;
        }
        entity target = zox_getv(pointer, Target);
        while (target && target != dropdown) {
            target = zox_get_parent(world, target);
        }
        if (!target) {
            dropdown_close(world, dropdown);
        }
    }
}
