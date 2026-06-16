// on the text entity
zox_sys2(AnimateTextSystem) {
    // zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AnimateTextTimeLimits);
    zox_sys_in(TargetText);
    zox_sys_out(AnimateTextBegin);
    zox_sys_out(AnimateTextTime);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    zox_sys_out(ZigelSpawnedDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(AnimateTextTimeLimits, new_time);
        zox_sys_i(TargetText, target);
        zox_sys_o(AnimateTextBegin, begin);
        zox_sys_o(AnimateTextTime, update_rate);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, text_dirty);
        zox_sys_o(ZigelSpawnedDirty, spawned_dirty);
        if (!begin->value) {
            continue;
        }
        // get index based on time
        double passed = zox_current_time - begin->value;
        if (passed > update_rate->value) {
            passed = update_rate->value;
        }
        if (passed < update_rate->value) {
            continue;
        }
        uint target_length = strlen(target->value);
        if (data->length >= target_length) {
            begin->value = 0;
            continue;
        }
        begin->value = zox_current_time;
        update_rate->value = rand_range(new_time->value.x, new_time->value.y);
        uint new_length = data->length + 1;
        // size_t animated_length = (passed / time->value) * length;
        // zox_log("Setting Text Target [%s] [%i/%i]", target->value, animated_length, length);
        if (!is_zext_cut(data, target->value, new_length)) {
            set_zext_cut(data, target->value, new_length);
            text_dirty->value = zox_dirty_trigger;
            spawned_dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(AnimateTextSystem);
