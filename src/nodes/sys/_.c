#include "start.c"
#include "next.c"
#include "end.c"

void zox_systems_nodes(ecs* world) {
    zox_system(
        node_start_system,
        zoxp_update,
        [none] nodes.NodeRun,
        [none] core.Initialize,
    );
    zox_system(
        next_node_system,
        zoxp_update,
        [none] nodes.NodeRun,
        [none] core.End,
    );
    zox_system(
        exit_node_system,
        zoxp_destroy,
        [none] nodes.NodeRun,
        [none] core.Exit,
    );
}
