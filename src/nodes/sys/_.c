#include "start.c"
#include "next.c"
#include "end.c"
realm_clear_system(NodegraphLinks);

void define_systems_nodes(ecs* world) {

    realm_clear_systemd(nodes, NodegraphLinks);

    zox_system(
        NodetreeBeginSystem,
        EcsOnUpdate,
        [in] nodes.NodeStartLink,
        [in] nodes.NodetreeBegin,
        [out] nodes.NodeLink
    );
    zox_system(
        NextNodeSystem,
        EcsOnUpdate,
        [in] nodes.NodeEnd,
        [out] nodes.NodeLink,
        [out] nodes.NodetreeEnd
    );
    zox_system(
        NodetreeEndSystem,
        EcsOnUpdate,
        [in] nodes.NodetreeEnd
    );
}
