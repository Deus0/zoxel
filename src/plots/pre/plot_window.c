entity spawn_prefab_plot_window(ecs *world) {
    zox_prefab_child(prefab_window_textured);
    zox_prefab_name("frame_debugger_ui");
    zox_add_tag(e, IgnoreWindowLayering);
    zox_prefab_set(e, PlotPaused, { 0 });
    zox_prefab_set(e, PlotDataDouble, { 0 });
    return e;
}
