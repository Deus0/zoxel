#include "start.c"
#include "next.c"

void define_systems_nodes(ecs* world) {
    zox_system(
        NodetreeBeginSystem,
        EcsOnUpdate,
        [in] NodetreeLink,
        [in] NodetreeBegin,
        [out] NodeLink
    );
    zox_system(
        NextNodeSystem,
        EcsOnUpdate,
        [in] NodeEnd,
        [out] NodeLink,
        [out] NodetreeEnd
    );
}