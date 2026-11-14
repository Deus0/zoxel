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
        entity data = zox_get_link(world, e, DataLink);
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
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity data = zox_get_link(world, e, DataLink);
        entity quest = data;
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
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity data = zox_get_link(world, e, DataLink);
        entity quest = data;
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
        // clicking the tracked quest untracks it
        const byte is_reset = zox_get_link(world, character, QuestTracked) == quest;
        track_quest(world, character, is_reset ? 0 : quest);
        if (!is_reset) {
            zox_link(world, quest, QuestIconLink, e);
        }
        // overlay shows we are tracking it
        entity overlay = zox_get_child_by_id(world, e, zox_id(IconOverlay));
        if (overlay) {
            zox_setv(overlay, RenderDisabled, is_reset);
            zox_setm(overlay, LocalScale1, !is_reset);
        } else {
            zox_loge("No IconOverlay on QuestIcon [%s]",
                zox_getn(e));
        }
        if (dbg_log) {
            zox_log("Quest [%s] %s", zox_getn(quest), is_reset ? "untracked" : "tracked");
        }
    }
}

// A quest log icon filled with its quest (DataUpdate): the tracked quest's icon shows it's tracked, its overlay on
//  (opening the log showed none tracked, only clicking set it)
void quest_icon_tracked_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity quest = zox_get_link(world, e, DataLink);
        if (!zox_valid(quest) || !zox_has(quest, Quest)) {
            continue;
        }
        const entity character = zox_get_parent(world, quest);
        if (!zox_valid(character) || zox_get_link(world, character, QuestTracked) != quest) {
            continue;
        }
        zox_link(world, quest, QuestIconLink, e);
        const entity overlay = zox_get_child_by_id(world, e, zox_id(IconOverlay));
        if (overlay) {
            zox_setv(overlay, RenderDisabled, 0);
        }
    }
}
