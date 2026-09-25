static inline void set_tooltip_text(
    ecs* world,
    entity ui,
    entity tooltip,
    const char* text)
{
    entity old_linked = zox_get_link(world, tooltip, TooltipLink);
    if (old_linked) {
        zox_loge("Tooltip Double Linked %s",
            zox_getn(old_linked));
        zox_unlink(world, tooltip, TooltipLink, old_linked);
    }
    // zox_log("Linking Tooltip [%s] => [%s]", zox_getn(tooltip), zox_getn(ui));
    // link tooltip new ui
    zox_link(world, tooltip, TooltipLink, ui);
    set_entity_text(world, tooltip, text);
}

byte tooltip_text_event(
    ecs* world,
    const TooltipEventData* data)
{
    if (!zox_valid(data->ui) || !zox_has(data->ui, TooltipText)) {
        zox_loge("Invalid Tooltip UI");
        return 0;
    }
    zox_geter(data->ui, TooltipText, tooltip_text);
    set_tooltip_text(
        world,
        data->ui,
        data->tooltip,
        tooltip_text->value);
    return 1;
}

static inline void zox_add_tooltip_text(
    ecs* world,
    entity ui,
    const char* text)
{
    zox_add(ui, Tooltipper);
    set_TooltipText(world, ui, text);
    zox_setv(ui, TooltipEvent, &tooltip_text_event);
}
