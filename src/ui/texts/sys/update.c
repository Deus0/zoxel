// just updates previous zigels to new data
zox_sys2(TextUpdateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(TextData, text);
        if (dirty->value != zox_dirty_active || !text->length) {
            continue;
        }
        entity children[texts_children_capacity];
        uint length = zox_get_children(world, e, children, texts_children_capacity);
        // zox_log("+ Text [%s] Dirty, Children [%i]", zox_get_name(e), length);
        for (uint j = 0; j < length; j++) {
            entity e2 = children[j];
            if (!zox_valid(e2)) {
                continue;
            }
            if (!zox_has(e2, ZigelIndex)) {
                zox_loge("Zigel [%s] does not have [ZigelIndex]", zox_get_name(e2));
                continue;
            }
            zox_mut_begin(e2, ZigelIndex, index);
            byte new_index = calculate_zigel_index(text->value, text->length, j);
            if (index->value != new_index) {
                zox_muter(e2, GenerateTexture, generate);
                index->value = new_index;
                generate->value = zox_dirty_trigger;
                zox_mut_end(e2, ZigelIndex);
                // zox_log("+ Text [%s]:[%i] New [%i] Old [%i]", zox_get_name(e), j, new_index, old_index);
            }
        }
        // zox_log("+ updating text [%s]", zox_get_name(e))
    }
} zox_sys_end(TextUpdateSystem);
