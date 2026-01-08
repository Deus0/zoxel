extern void hide_virtual_keyboard();

void HideVirtualKeyboardSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(ZevicePointer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ZevicePointer, pointer);
        if (pointer->value) {
            hide_virtual_keyboard();
        }
    }
} zoxd_system2(HideVirtualKeyboardSystem);