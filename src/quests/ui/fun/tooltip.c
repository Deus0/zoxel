byte tooltip_event_quest(
    ecs *world,
    const TooltipEventData *data
) {
    if (!data->data || !zox_has(data->data, Quest)) {
        return 0;
    }
    // set_entity_text(world, data->tooltip, "selected skill");
    byte used_name = 0;
    char result[64];
    if (data->data && zox_has(data->data, ZoxName)) {
        zox_geter(data->data, ZoxName, zox_name);
        if (zox_name && zox_name->length > 0) {
            char *name_string = convert_zext_to_text(
                zox_name->value,
                zox_name->length
            );
            if (name_string != NULL) {
                used_name = 1;

                // TODO: Get Objectives
                if (zox_has(data->data, SlayQuest)) {
                    zox_geter_value(data->data, CharacterLink, entity, character);
                    zox_geter_value(data->data, QuestValue, byte, value);
                    zox_geter_value(data->data, QuestTarget, byte, target);
                    sprintf(result, "[%s]: slay [%s] [%i/%i]", name_string, zox_get_name(character), value, target);
                } else {
                    sprintf(result, "[%s]: find ???", name_string);
                }
                free(name_string);
            }
        }
    }
    if (!used_name) {
        sprintf(result, "[%s] q\n", zox_get_name(data->data));
    }
    set_entity_text(world, data->tooltip, result);
    return 1;
}