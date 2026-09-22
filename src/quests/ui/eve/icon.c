// Tooltip Event for item icons
// NOTE: Quests have no slots, just direct data linked to ui
void quest_icon_label_event(iter* it) {
    byte dbg_log = 1;
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, text);
        entity data = zox_get_link(world, e, DataLink);
        if (!data || !zox_has(data, Quest)) {
            if (data && dbg_log) {
                zox_loge("Data not quest [%s]",
                    zox_getn(data));
            }
            continue;
        }
        char result[label_text_capacity];
        if (zox_has(data, SlayQuest)) {
            byte value = zox_getv(data, QuestValue);
            byte target = zox_getv(data, QuestTarget);
            sprintf(result, "%i/%i", value, target);
        } else {
            sprintf(result, "test");
            // result[0] = '\0';
        }
        if (!is_zext(text, result)) {
            set_zext(text, result);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Quest Frame Label [%s] Set to [%s]",
                    zox_getn(e),
                    result);
            }
        }
        /*byte quantity =
            zox_has(data, Quantity) ?
                zox_getv(data, Quantity) :
                0;
        char result[label_text_capacity];
        if (quantity > 1) {
            snprintf(result, label_text_capacity, "x%i", quantity);
        } else {
            result[0] = '\0';
        }
        // set text of quantity label
        if (!is_zext(text, result)) {
            set_zext(text, result);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Item Frame Label [%s] Set to [%s]",
                    zox_get_name(e),
                    result);
            }
        }*/
    }
}
