// on the text entity, main thread: the next button pops in (ui_pop_in spawns a tween)
void animate_text_end_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnimateTextEnded);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AnimateTextEnded, ended);
        if (ended->value != zox_dirty_active) {
            continue;
        }
        entity window = zox_get_parent_by_id(
            world,
            e,
            zox_id(Window));
        entity next_button = zox_get_child_by_id_recursive(
            world,
            window,
            zox_id(DialogueButton));
        if (!zox_valid(next_button)) {
            zox_loge("Could not find next_button for Dialogue Text [%s]", zox_get_name(e));
            continue;
        }
        set_children_by_id_byte(
            world,
            next_button,
            zox_id(RenderDisabled),
            0);
        ui_pop_in(world, next_button);
    }
    zox_sys_on_end();
} zoxd_system(animate_text_end_system);
