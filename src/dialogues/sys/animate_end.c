// on the text entity, main thread: the next button pops in (ui_pop_in spawns a tween)
// an observer on AnimateTextEnded (non fragmenting, removed after)
void animate_text_end_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, AnimateTextEnded);
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
}
