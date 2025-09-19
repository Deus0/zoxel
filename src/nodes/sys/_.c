#include "start.c"
#include "next.c"
#include "end.c"

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
    zox_system(
        NodetreeEndSystem,
        EcsOnUpdate,
        [in] NodetreeEnd
    );
}