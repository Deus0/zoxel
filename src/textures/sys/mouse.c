zox_sys2(MouseTextureSystem) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(MouseLock)
    // zox_sys_in(TextureLink)
    zox_sys_in(DeviceDisabled)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MouseLock, mouseLock);
        zox_sys_i(DeviceDisabled, deviceDisabled);
        entity texture = zox_get_link(world, e, TextureLink);
        if (!zox_valid(texture)) {
            continue;
        }
        byte render_disabled = mouseLock->value || deviceDisabled->value;
        zox_setv(texture, RenderDisabled, render_disabled);
    }
} zox_sys_end(MouseTextureSystem);
