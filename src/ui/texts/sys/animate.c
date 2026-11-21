// Types a text out a character at a time (TargetText), on the text entity (i.e. dialogue)
void animate_text_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TargetText);
    zox_sys_in(AnimateTextTimeLimits);
    zox_sys_out(AnimateTextBegin);
    zox_sys_out(AnimateTextTime);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TargetText, target);
        zox_sys_i(AnimateTextTimeLimits, new_time);
        zox_sys_o(AnimateTextBegin, begin);
        zox_sys_o(AnimateTextTime, rate);
        zox_sys_o(TextData, data);
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
            zox_add(e, GlyphSpawnedDirty);
            zox_add(e, Refresh);
            if (target_length == new_length) {
                begin->value = 0;
                zox_add(e, AnimateTextEnded);
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
    zox_sys_on_end();
} zoxd_system(animate_text_system);
