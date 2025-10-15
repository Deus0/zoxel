// todo: add raycaster onto each pointer zevice (for multiple fingers) - refactor from player entity and add Raycaster onto each zevice_pointer! This way we can click buttons while using joystick on the other finger

void ZeviceRaycasterSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(ZevicePointerPosition);
    zox_sys_out(Raycaster);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ZevicePointerPosition, position);
        zox_sys_o(Raycaster, raycaster);
        raycaster->value = position->value;
    }
} zoxd_system2(ZeviceRaycasterSystem);