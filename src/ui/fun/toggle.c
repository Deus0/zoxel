entity player_toggle_ui(
    ecs *world,
    entity player,
    entity e,
    entity (*spawn_ui)(ecs*, const entity))
{
    // one popping out reads as closed (UIClosing), a new one opens
    if (!zox_valid(e) || zox_has(e, UIClosing)) {
        return (*spawn_ui)(world, player);
    }
    ui_pop_out(world, e);
    return 0;
}

entity toggle_ui_with_id(
    ecs *world,
    entity (*spawn_ui)(ecs*, entity),
    entity id,
    entity player)
{
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity ui = zox_get_child_by_id(world, canvas, id);
    // find_child_with_id(canvas, id, ui)
    return player_toggle_ui(world, player, ui, spawn_ui);
}

void toggle_ui(
    ecs *world,
    entity canvas,
    entity *e,
    entity (*spawn_ui)(ecs*, entity))
{
    if (!zox_valid(*e) || zox_has(*e, UIClosing)) {
        *e = (*spawn_ui)(world, canvas);
    } else {
        ui_pop_out(world, *e);
        *e = 0;
    }
}
