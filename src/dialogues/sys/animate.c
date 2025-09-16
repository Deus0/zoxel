// on the text entity
void AnimateText2System(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnimateTextBegin);
    zox_sys_in(AnimateTextTime);
    zox_sys_in(TargetText);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(AnimateTextBegin, begin);
        zox_sys_i(AnimateTextTime, time);
        zox_sys_i(TargetText, target);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);

        if (!begin->value) {
            continue;
        }

        // get index based on time
        double passed = zox_current_time - begin->value;
        if (passed > time->value) {
            passed = time->value;
        }
        size_t length = strlen(target->value);
        size_t animated_length = (passed / time->value) * length;


        zox_log("Ssetting Text Target [%s] [%i/%i]", target->value, animated_length, length);

        if (!is_zext_cut(data, target->value, animated_length)) {
            set_zext_cut(data, target->value, animated_length);
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(AnimateText2System);