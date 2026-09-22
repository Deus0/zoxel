// Tooltip Event for item icons
void quest_icon_tooltip_event(iter* it) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, datac);
        /*if (state->value != zox_state_select_active) {
            continue;
        }*/
        // zox_log("quest_icon_tooltip_event [%s]", zox_sys_e_name);
        entity data = datac->value;
        // TODO: We should do this for only X types of items
        //      - BlockItem, ConsumeableItem, etc
        if (!zox_valid(data) ||
            !zox_has(data, Quest))
        {
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
            zox_has(data, ZoxName) ?
                zox_getv(data, ZoxName) :
                zox_getn(data);
        char result[128];
        // TODO: Get Objectives
        if (zox_has(data, SlayQuest)) {
            entity character = zox_get_link(world, data, CharacterLink);
            const char* character_name =
                zox_has(character, ZoxName) ?
                    zox_getv(character, ZoxName) :
                    zox_getn(character);
            byte value = zox_getv(data, QuestValue);
            byte target = zox_getv(data, QuestTarget);
            sprintf(
                result,
                "%s\n- Slay [%s] x%i",
                quest_name,
                character_name,
                target);
        } else {
            sprintf(
                result,
                "%s\n- Find [???]",
                quest_name);
        }
        set_tooltip_text(world, e, tooltip, result);
        /*byte quantity =
            zox_has(data, Quantity) ?
                zox_getv(data, Quantity) :
                1;
        char result[128];
        sprintf(
            result,
            "[%s] x%i\n",
            name,
            quantity);*/
    }
}
