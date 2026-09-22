/*zox_sys2(QuestIconLabelSystem) {
    byte label_text_capacity = 12;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, data);
        zox_sys_o(TextData, text);
        if (!zox_valid(data->value)) {
            continue;
        }
        entity e2 = data->value;
        if (!zox_valid(e2) || !zox_has(e2, Quest) || !zox_has(e2, QuestDirty)) {
            continue;
        }
        zox_geter_value(e2, QuestDirty, byte, qdirty);
        if (text->length && qdirty != zox_dirty_active) {
            continue;
        }
        char result[label_text_capacity];
        if (zox_has(e2, SlayQuest)) {
            zox_geter_value(e2, QuestValue, byte, value);
            zox_geter_value(e2, QuestTarget, byte, target);
            sprintf(result, "%i/%i", value, target);
        } else {
            result[0] = '\0';
        }
        if (!is_zext(text, result)) {
            set_zext(text, result);
            zox_add(e, Dirty);
        }
    }
} zox_sys_end(QuestIconLabelSystem);*/
