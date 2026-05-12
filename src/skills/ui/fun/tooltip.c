byte tooltip_event_skill(ecs *world, const TooltipEventData* data) {
    if (!data->data || !zox_has(data->data, Skill)) {
        return 0;
    }
    byte used_name = 0;
    char result[128];
    if (data->data && zox_has(data->data, ZoxName)) {
        zox_geter(data->data, ZoxName, zox_name);
        used_name = 1;
        sprintf(result, "[%s] lvl [1]\n", zox_name->value);
    }
    if (!used_name) {
        sprintf(result, "[%s]\n", zox_get_name(data->data));
    }
    set_entity_text(world, data->tooltip, result);
    // zox_log(" > tooltip icon skill set [%lu] selected at %f\n", data->tooltip, zox_current_time)
    return 1;
}
