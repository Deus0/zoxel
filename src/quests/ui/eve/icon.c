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

// Tooltip Event for item icons
void quest_icon_tooltip_event(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, data);
        entity quest = data->value;
        if (!zox_valid(quest) ||
            !zox_has(quest, Quest))
        {
            if (dbg_log) {
                zox_loge("Dirty Data not Quest [%s]",
                    zox_getn(quest));
            }
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(
            world,
            canvas,
            zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        const char* quest_name =
            zox_has(quest, ZoxName) ?
                zox_getv(quest, ZoxName) :
                zox_getn(quest);
        uint index = 0;
        char text[256];
        // TODO: Get Objectives
        if (zox_has(quest, SlayQuest)) {
            entity character = zox_get_link(world, quest, CharacterLink);
            const char* character_name =
                zox_has(character, ZoxName) ?
                    zox_getv(character, ZoxName) :
                    zox_getn(character);
            byte value = zox_getv(quest, QuestValue);
            byte target = zox_getv(quest, QuestTarget);
            index += sprintf(
                index + text,
                "%s\n- Slay [%s] [%i/%i]",
                quest_name,
                character_name,
                value,
                target);
        } else {
            index += sprintf(
                index + text,
                "%s\n- Find [???]",
                quest_name);
        }
        if (zox_has(quest, QuestHandedin)) {
            index += sprintf(
                index + text,
                "\n- Quest Handed In");
        } else if (zox_has(quest, QuestDone)) {
            index += sprintf(
                index + text,
                "\n- Hand In Quest");
        }
        set_tooltip_text(world, e, tooltip, text);
        if (dbg_log) {
            zox_log("Quest Tooltip %s: %s",
                zox_getn(quest),
                text);
        }
        /*byte quantity =
            zox_has(quest, Quantity) ?
                zox_getv(quest, Quantity) :
                1;
        char result[128];
        sprintf(
            result,
            "[%s] x%i\n",
            name,
            quantity);*/
    }
}
