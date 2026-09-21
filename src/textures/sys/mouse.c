zox_sys2(MouseTextureSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MouseLock);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MouseLock, mouseLock);
        entity texture = zox_get_link(world, e, TextureLink);
        if (!zox_valid(texture)) {
            continue;
        }
        byte render_disabled =
            mouseLock->value ||
            zox_has(e, Disabled);
        zox_setv(texture, RenderDisabled, render_disabled);
    }
} zox_sys_end(MouseTextureSystem);
