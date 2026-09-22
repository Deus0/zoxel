// Tooltip Event for item icons
// NOTE: Quests have no slots, just direct data linked to ui
void quest_icon_label_event(iter* it) {
    byte dbg_log = 0;
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, text);
        entity data = zox_has(e, DataLink) ?
            zox_getv(e, DataLink) :
            0;
        // zox_get_link(world, e, DataLink);
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
            result[0] = '\0';
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
    }
}
