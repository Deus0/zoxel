entity player_toggle_ui(
    ecs *world,
    entity player,
    entity e,
    entity (*spawn_ui)(ecs*, const entity))
{
    if (e == 0) {
        return (*spawn_ui)(world, player);
    } else {
        zox_delete(e);
        return 0;
    }
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
    if (*e == 0) {
        entity e2 = (*spawn_ui)(world, canvas);
        *e = e2;
    } else {
        zox_delete(*e);
        *e = 0;
    }
}
