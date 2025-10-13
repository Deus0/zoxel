void update_text(
    ecs *world,
    const Children* children,
    const TextData* textData
) {
    // calculate checks for spaces new lines etc and removes those
    const int new_children_length = calculate_total_zigels(textData->value, textData->length);
    const int update_count = int_min(children->length, new_children_length);
    for (int i = 0; i < update_count; i++) {
        const entity e = children->value[i];
        if (!zox_valid(e)) {
            continue;
        }
        const byte new = calculate_zigel_index(textData->value, textData->length, i);
        zox_geter_value(e, ZigelIndex, byte, old);
        if (old != new) {
            zox_muter(e, ZigelIndex, index);
            zox_muter(e, GenerateTexture, generate);
            index->value = new;
            generate->value = zox_dirty_trigger;
            zox_log_text("    + zigel [%i] updated [%i:%i]", i, old, new);
        }
    }
}

// just updates previous zigels to new data
void TextUpdateSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(Children);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextDirty, zextDirty);
        zox_sys_i(Children, children);
        zox_sys_i(TextData, textData);

        if (zextDirty->value != zox_dirty_active || !textData->length) {
            continue;
        }

        update_text(world, children, textData);
        // zox_sys_e()
        // zox_log("+ updating text [%s]", zox_get_name(e))
    }
} zoxd_system2(TextUpdateSystem);