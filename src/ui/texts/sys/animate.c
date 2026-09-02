/*zox_sys2(AnimateTextSystem) {
    init_delta_time()
    zox_sys_begin()
    zox_sys_out(AnimateText);
    zox_sys_out(TextDirty);
    zox_sys_out(TextData);
    byte changed = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(AnimateText, animateText);
        zox_sys_o(TextDirty, zextDirty);
        zox_sys_o(TextData, textData);
        animateText->value -= delta_time;
        if (animateText->value <= 0.0) {
            animateText->value += zext_animation_speed;
            if (animateText->value <= -zext_animation_speed) {
                animateText->value = 0;
            }
            if (zextDirty->value == 0) {
                const int index = rand() % textData->length;
                textData->value[index] = 1 + rand() % 52;
                changed = 1;
                zextDirty->value = 1;
#ifdef zoxel_debug_zext_updates
                zox_log("AnimateText :: [%lu]\n", it->entities[i])
#endif
            }
        }
    }
    if (!changed) zox_skip()
} zox_sys_end(AnimateTextSystem);*/
