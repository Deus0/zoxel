byte tooltip_event_item(ecs *world, const TooltipEventData *data) {
    if (!data->data || !zox_has(data->data, Item)) {
        // zox_log("No Data on tooltip.");
        return 0;
    }
    char result[128];
    const byte quantity = zox_has(data->data, Quantity) ? zox_gett_value(data->data, Quantity) : 0;
    if (zox_has(data->data, ZoxName)) {
        zox_geter(data->data, ZoxName, zox_name);
        sprintf(result, "[%s] x%i\n", zox_name->value, quantity);
    } else {
        sprintf(result, "[%s] x%i\n", zox_get_name(data->data), quantity);
    }
    set_entity_text(world, data->tooltip, result);
    return 1;
}