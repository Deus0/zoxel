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

// Click
void quest_icon_click_event(iter* it) {
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
        if (dbg_log) {
            zox_log("Quest Clicked %s",
                zox_getn(quest));
        }
        entity character = zox_get_parent(world, quest);
        if (!zox_valid(character)) {
            zox_loge("Quest has no Character.");
            continue;
        }
        byte is_reset = 0;
        entity old_quest = zox_get_link(world, character, QuestTracked);
        if (old_quest == quest) {
            is_reset = 1;
            if (dbg_log) {
                zox_log("QuestTracker Same Quest",
                    zox_getn(quest));
            }
            // Get ui linked to quest here and remove its overlay
        }
        if (old_quest) {
            zox_unlink(world, character, QuestTracked, old_quest);
            entity old_icon = zox_get_link(world, old_quest, QuestIconLink);
            if (old_icon) {
                zox_unlink(world, old_quest, QuestIconLink, old_icon);
                entity old_overlay = zox_get_child_by_id(world, old_icon, zox_id(IconOverlay));
                if (old_overlay) {
                    zox_setv(old_overlay, RenderDisabled, 1);
                }
            }
        }
        if (!is_reset) {
            zox_link(world, character, QuestTracked, quest);
            zox_link(world, quest, QuestIconLink, e);
        }
        // Overlay to show we are tracking it
        entity overlay = zox_get_child_by_id(world, e, zox_id(IconOverlay));
        if (overlay) {
            zox_setv(overlay, RenderDisabled, is_reset);
            zox_setm(overlay, LocalScale1, !is_reset);
            if (dbg_log) {
                zox_log("Quest Icon Overlay RenderDisabled set to [%i]",
                    is_reset);
            }
        } else {
            zox_loge("No IconOverlay on QuestIcon [%s]",
                zox_getn(e));
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tracker = zox_get_child_by_id(
            world,
            canvas,
            zox_id(QuestTracker));
        if (!tracker) {
            continue;
        }
        if (!is_reset) {
            zox_link(world, quest, QuestTrackerLink, tracker);
        }
        // TODO: Link to tracker ui
        char* text = "";
        if (!is_reset) {
            if (quest) {
                text = (char*) get_quest_tracker_text(world, quest);
            }
        }
        if (set_entity_text(world, tracker, text)) {
            if (dbg_log) {
                zox_log("QuestTracker [%s] Set to [%s]",
                    zox_getn(tracker),
                    text);
            }
        }
    }
}
