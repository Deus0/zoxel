zox_sys2(DestroyInFrameSystem) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_delete(e);
    }
} zox_sys_end(DestroyInFrameSystem);
