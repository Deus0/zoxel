byte tooltip_event_quest(ecs *world, const TooltipEventData *data) {
    if (!data->data || !zox_has(data->data, Quest)) {
        return 0;
    }
    // set_entity_text(world, data->tooltip, "selected skill");
    char result[128];
    if (data->data && zox_has(data->data, ZoxName)) {
        zox_geter(data->data, ZoxName, zox_name);
        // TODO: Get Objectives
        if (zox_has(data->data, SlayQuest)) {
            zox_geter_value(data->data, CharacterLink, entity, character);
            zox_geter_value(data->data, QuestValue, byte, value);
            zox_geter_value(data->data, QuestTarget, byte, target);
            sprintf(result, "[%s]: slay [%s] [%i/%i]", zox_name->value, zox_get_name(character), value, target);
        } else {
            sprintf(result, "[%s]: find ???", zox_name->value);
        }
    } else {
        sprintf(result, "[%s] q\n", zox_get_name(data->data));
    }
    set_entity_text(world, data->tooltip, result);
    return 1;
}