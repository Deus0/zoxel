#include "generate.c"

void zox_systems_names(ecs* world) {
    zox_system(
        GenerateNameSystem,
        zoxp_update,
        [in] core.Seed,
        [out] names.GenerateName,
        [out] core.ZoxName
    );
}
