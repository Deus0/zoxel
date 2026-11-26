static void zox_events_docks(ecs* world) {
    zox_on_add(dock_added_event, [none] docks.Docked);
    zox_observe_disabled(dock_removed_event, EcsOnRemove, [none] docks.Docked);
    zox_on_add(dock_dirty_event, [none] docks.Docked, [in] layouts.LayoutSize, [none] core.Dirty);
    zox_on_add(docks_layout_event, [none] layouts.Canvas, [none] core.Dirty);
    zox_on_add(dock_drag_event, [none] interactions.Dragable, [none] core.Dirty);
}
