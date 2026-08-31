#include "start.c"
#include "next.c"
#include "end.c"
// realm_clear_system(NodegraphLinks);

void define_systems_nodes(ecs* world) {
    // realm_clear_systemd(nodes, NodegraphLinks);
    zox_system(
        NodetreeBeginSystem,
        zoxp_update,
        [in] nodes.NodeStartLink,
        [in] nodes.NodetreeBegin,
        [out] nodes.NodeLink
    );
    zox_system(
        NextNodeSystem,
        zoxp_update,
        [in] nodes.NodeEnd,
        [out] nodes.NodeLink,
        [out] nodes.NodetreeEnd
    );
    zox_system(
        NodetreeEndSystem,
        zoxp_update,
        [in] nodes.NodetreeEnd
    );
}
