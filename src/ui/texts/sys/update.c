/*void update_text(ecs *world, const Children* children, const TextData* textData) {

    // calculate checks for spaces new lines etc and removes those
    int new_children_length = calculate_total_zigels(textData->value, textData->length);
    int update_count = int_min(children->length, new_children_length);

    for (int i = 0; i < update_count; i++) {
        entity e = children->value[i];

        if (!zox_valid(e)) {
            continue;
        }

        byte new = calculate_zigel_index(textData->value, textData->length, i);
        zox_geter_value(e, ZigelIndex, byte, old);

        if (old != new) {
            zox_muter(e, ZigelIndex, index);
            zox_muter(e, GenerateTexture, generate);
            index->value = new;
            generate->value = zox_dirty_trigger;
            zox_log_text("    + zigel [%i] updated [%i:%i]", i, old, new);
        }
    }
}*/

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

            zox_geter_value(e2, ZigelIndex, byte, old_index);
            byte new_index = calculate_zigel_index(text->value, text->length, j);
            if (old_index != new_index) {
                zox_muter(e2, ZigelIndex, index);
                zox_muter(e2, GenerateTexture, generate);
                index->value = new_index;
                generate->value = zox_dirty_trigger;
                // zox_log("+ Text [%s]:[%i] New [%i] Old [%i]", zox_get_name(e), j, new_index, old_index);
            }
        }


        // update_text(world, children, text);
        // zox_sys_e()
        // zox_log("+ updating text [%s]", zox_get_name(e))
    }
} zox_sys_end(TextUpdateSystem);
