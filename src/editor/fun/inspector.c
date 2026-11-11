extern entity spawn_inspector(ecs*, entity, entity, entity);

// the canvas's inspector window, 0 if it's closed
entity get_canvas_inspector(
    ecs* world,
    entity canvas)
{
    const entity inspector = zox_get_child_by_id(world, canvas, zox_id(InspectorUI));
    return zox_valid(inspector) ? inspector : 0;
}

// points the inspector at target, it respawns its rows, returns 0 if it already was
byte set_inspector_target(
    ecs* world,
    entity inspector,
    entity target)
{
    if (zox_getv(inspector, EntityTarget) == target) {
        return 0;
    }
    zox_setv(inspector, EntityTarget, target);
    zox_setv(inspector, InspectorDirty, zox_dirty_trigger);
    zox_logv("+ Inspector Target [%s]", zox_valid(target) ? zox_getn(target) : "None");
    return 1;
}

// inspects target in the player's inspector, opening it if closed (i.e. from the hierarchy)
void inspector_select_target(
    ecs* world,
    entity player,
    entity target)
{
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        return;
    }
    const entity inspector = get_canvas_inspector(world, canvas);
    if (!inspector) {
        spawn_inspector(world, canvas, player, target);
    } else {
        set_inspector_target(world, inspector, target);
    }
}

// inspects e if the debug player's inspector is open (used by debug spawners)
void inspect_entity(
    ecs* world,
    entity e)
{
    const entity canvas = get_editor_canvas(world);
    const entity inspector = canvas ? get_canvas_inspector(world, canvas) : 0;
    if (inspector) {
        set_inspector_target(world, inspector, e);
    }
}
