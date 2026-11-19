static inline void set_tooltip_text(
    ecs* world,
    entity ui,
    entity tooltip,
    const char* text)
{
    // NOTE: moving straight from one ui to the next, the old one's deselect unlink is still
    //  deferred, so it's still linked here, that's expected: the link replaces it (TooltipLink is exclusive)
    //  - not unlinked first: the remove of the old one, deferred with the new link, dropped the new one too (stepping
    //      on with a gamepad, every other ui had no tooltip)
    entity old_linked = zox_get_link(world, tooltip, TooltipLink);
    if (old_linked == ui) {
        return;
    }
    // zox_log("Linking Tooltip [%s] => [%s]", zox_getn(tooltip), zox_getn(ui));
    // link tooltip new ui
    zox_link(world, tooltip, TooltipLink, ui);
    set_entity_text(world, tooltip, text);
    if (zox_log_tooltips) {
        zox_log("Tooltip [%s] UI [%s] set to [%s]",
            zox_getn(tooltip),
            zox_getn(ui),
            text);
    }
}

byte tooltip_text_event(
    ecs* world,
    const TooltipEventData* data)
{
    if (!zox_valid(data->ui) ||
        !zox_has(data->ui, TooltipText))
    {
        zox_loge("Invalid Tooltip UI");
        return 0;
    }
    const TooltipText* tooltip_text = zox_get(data->ui, TooltipText);
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
