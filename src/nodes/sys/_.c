#include "start.c"
#include "next.c"
#include "end.c"
#include "events.c"

void zox_systems_nodes(ecs* world) {
    zox_system(
        NodetreeBeginSystem,
        zoxp_update,
        [in] nodes.NodeStartLink,
        [in] nodes.NodetreeBegin,
        [none] nodes.NodeRun,
    );
    zox_system(
        NextNodeSystem,
        zoxp_update,
        [in] nodes.NodeEnd,
        [out] nodes.NodetreeEnd,
        [none] nodes.NodeRun,
    );
    zox_system(
        NodetreeEndSystem,
        zoxp_update,
        [in] nodes.NodetreeEnd,
        [none] nodes.NodeRun,
    );
    zox_system_1(
        NodeBeginEventSystem,
        zoxp_spawn,
        [in] nodes.NodeBegin,
        [none] nodes.NodeRun,
    );
}
