// The inspector row tooltip: the component's name and type, and for children / entities
// components, the entities they hold (cut off with ... when the tooltip is full)

// keeps index inside text after an snprintf that may have been cut off
static inline int clamp_tooltip_index(int index, int size) {
    return index < size ? index : size - 1;
}

// appends a line per entity to text, returns the new index, stops with "..." when full
static inline int append_tooltip_entity_line(
    ecs* world,
    char* text,
    int size,
    int index,
    entity e)
{
    // leaves room for the "..." line
    const int reserve = 8;
    if (index < 0 || index >= size - reserve) {
        return index;
    }
    const int written = snprintf(text + index, size - reserve - index, "\n  - %s", zox_valid(e) ? zox_get_name(e) : "None");
    if (written >= size - reserve - index) {
        // cut off, ends at the reserve
        index = size - reserve - 1;
        index += snprintf(text + index, size - index, "\n  ...");
        return size;
    }
    return index + written;
}

byte inspector_label_tooltip(
    ecs *world,
    const TooltipEventData *data)
{
    const entity label = data->ui;
    const entity tooltip = data->tooltip;
    if (!zox_valid(label) || !zox_has(label, TooltipText)) {
        zox_loge("Invalid Tooltip UI");
        return 0;
    }
    const byte ctype = zox_getv(label, ComponentType);
    if (ctype >= zox_type_end) {
        const TooltipText* text = zox_get(label, TooltipText);
        set_entity_text(world, tooltip, text->value);
        return 1;
    }
    const entity e = zox_getv(label, EntityTarget);
    const entity cid = zox_getv(label, ComponentTarget);
    const int size = TooltipText_length;
    char text[TooltipText_length];
    int index = snprintf(text, size, "%s [%s]",
        zox_valid(cid) ? zox_get_name(cid) : (zox_valid(e) ? zox_get_name(e) : "Invalid"),
        zox_type_labels[ctype]);
    index = clamp_tooltip_index(index, size);
    if (ctype == zox_type_children && zox_valid(e)) {
        index = clamp_tooltip_index(index + snprintf(text + index, size - index, " len [%i]", zox_get_children_count(world, e)), size);
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && index < size; j++) {
                index = append_tooltip_entity_line(world, text, size, index, it2.entities[j]);
            }
        }
    } else if (ctype == zox_type_entities && zox_valid(e) && zox_valid(cid)) {
        const entities* links = (const entities*) zox_get_id(e, cid);
        index = clamp_tooltip_index(index + snprintf(text + index, size - index, " len [%i]", links->length), size);
        for (int i = 0; i < links->length && index < size; i++) {
            index = append_tooltip_entity_line(world, text, size, index, links->value[i]);
        }
    } else if (ctype == zox_type_unknown) {
        snprintf(text + index, size - index, " ?");
    }
    set_entity_text(world, tooltip, text);
    return 1;
}
