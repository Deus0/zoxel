byte tooltip_event_stat(ecs *world, const TooltipEventData *data) {
    if (!data->data || !zox_has(data->data, Stat)) {
        if (data->data) {
            zox_log("e [%s] has no Stat", zox_get_name(data->data));
        }
        return 0;
    }
    char result[128];
    const char *name = zox_has(data->data, ZoxName) ? zox_gett_value(data->data, ZoxName) : "none";
    if (zox_has(data->data, StatLevel)) {
        float value = zox_get_value(data->data, StatValue)
        float experience = zox_get_value(data->data, ExperienceValue)
        float experience_max = zox_get_value(data->data, ExperienceMax)
        sprintf(result, "[%s] lvl %i [%i/%i]\n", name, (int) value, (int) experience, (int) experience_max);
    } else if (zox_has(data->data, StatState)) {
        float value = zox_get_value(data->data, StatValue)
        float value_max = zox_get_value(data->data, StatValueMax)
        sprintf(result, "[%s] [%i/%i]\n", name, (int) value, (int) value_max);
    } else if (zox_has(data->data, StatRegen)) {
        float value = zox_get_value(data->data, StatValue)
        sprintf(result, "[%s] [%i]\n", name, (int) value);
    } else if (zox_has(data->data, StatAttribute)) {
        float value = zox_get_value(data->data, StatValue)
        sprintf(result, "[%s] [%i]\n", name, (int) value);
    } else {
        sprintf(result, "[%s]\n", name);
    }
    set_entity_text(world, data->tooltip, result);
    zox_set(data->tooltip, RenderDisabled, { 0 });
    return 1;
}
