void DestroyInFrameSystem(iter *it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_delete(e);
    }
} zoxd_system2(DestroyInFrameSystem);
