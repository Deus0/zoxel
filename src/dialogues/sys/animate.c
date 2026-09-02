// on the text entity
zox_sys2(AnimateTextSystem) {
    byte dbg_log = 0;
    // zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TargetText);
    zox_sys_in(AnimateTextTimeLimits);
    zox_sys_out(AnimateTextBegin);
    zox_sys_out(AnimateTextTime);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    zox_sys_out(GlyphSpawnedDirty);
    zox_sys_out(AnimateTextEnded);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TargetText, target);
        zox_sys_i(AnimateTextTimeLimits, new_time);
        zox_sys_o(AnimateTextBegin, begin);
        zox_sys_o(AnimateTextTime, rate);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, text_dirty);
        zox_sys_o(GlyphSpawnedDirty, spawned_dirty);
        zox_sys_o(AnimateTextEnded, ended);
        if (!begin->value) {
            if (dbg_log >= 2) {
                zox_log("AnimateTextBegin Not Started");
            }
            continue;
        }
        // get index based on time
        double passed = zox_current_time - begin->value;
        if (passed > rate->value) {
            passed = rate->value;
        }
        if (passed < rate->value) {
            if (dbg_log >= 2) {
                zox_log("AnimateTextBegin Waiting %f < %f",
                    passed,
                    rate->value);
            }
            continue;
        }
        uint target_length = strlen(target->value);
        if (data->length >= target_length) {
            begin->value = 0;
            if (dbg_log) {
                zox_log("Reached End of Text [%s]", target->value);
            }
            continue;
        }
        rate->value = frand_range(new_time->value.x, new_time->value.y);
        if (dbg_log) {
            zox_log("Next Update Rate [%f] from [%f,%f]",
                rate->value,
                new_time->value.x,
                new_time->value.y);
        }
        uint new_length = data->length + 1;
        // size_t animated_length = (passed / time->value) * length;
        // zox_log("Setting Text Target [%s] [%i/%i]", target->value, animated_length, length);
        if (!is_zext_cut(data, target->value, new_length)) {
            set_zext_cut(data, target->value, new_length);
            text_dirty->value = zox_dirty_trigger;
            spawned_dirty->value = zox_dirty_trigger;
            if (target_length == new_length) {
                begin->value = 0;
                ended->value = zox_dirty_trigger;
                if (dbg_log) {
                    zox_log("Text Ended [%s] : Length [%i]",
                        target->value,
                        new_length);
                }
            } else {
                // NOTE: Continue Animating
                begin->value = zox_current_time;
                if (dbg_log) {
                    zox_log("Text Updated [%s] : Length [%i]",
                        target->value,
                        new_length);
                }
            }
        } else {
            begin->value = 0;
            if (dbg_log >= 2) {
                zox_log("Text Not Updated [%s] : Length [%i]",
                    target->value,
                    new_length);
            }
        }
    }
} zox_sys_end(AnimateTextSystem);
