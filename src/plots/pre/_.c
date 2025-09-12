entity prefab_frame_debugger_ui;

#include "plot_window.c"
#include "plot_graph.c"

void spawn_prefabs_plots(ecs *world) {
    prefab_frame_debugger_ui = spawn_prefab_frame_debugger_ui(world);
}