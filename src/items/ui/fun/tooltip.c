byte tooltip_event_item(
    ecs *world,
    const TooltipEventData *data
) {
    if (!data->data || !zox_has(data->data, Item)) {
        return 0;
    }
    char result[64];
    const ZoxName *zox_name = zox_get(data->data, ZoxName)
    const byte quantity = zox_has(data->data, Quantity) ? zox_gett_value(data->data, Quantity) : 0;
    if (zox_name && zox_name->value && zox_name->length) {
        char *name_string = convert_zext_to_text(zox_name->value, zox_name->length);
        sprintf(result, "[%s] x%i\n", name_string, quantity);
        free(name_string);
    } else {
        sprintf(result, "[%s] x%i\n", zox_get_name(data->data), quantity);
    }
    set_entity_text(world, data->tooltip, result);
    return 1;
}