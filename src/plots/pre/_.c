//entity prefab_plot_window;
entity prefab_plot_graph;

// #include "plot_window.c"
#include "plot_graph.c"

void spawn_prefabs_plots(ecs *world) {
    prefab_plot_graph = spawn_prefab_plot_graph(world, prefab_layout2);
    //prefab_plot_window = spawn_prefab_plot_window(world);
}
