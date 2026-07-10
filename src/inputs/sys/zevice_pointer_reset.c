zox_sys2(ZevicePointerResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZevicePointer);
    zox_sys_out(ZevicePointerOld);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZevicePointer, zevicePointer);
        zox_sys_o(ZevicePointerOld, zevicePointerOld);
        zevicePointerOld->value = zevicePointer->value;
        zevicePointer->value = reset_button_state(zevicePointer->value);
    }
} zox_sys_end(ZevicePointerResetSystem);