#include "generate.c"

void zox_define_systems_names(ecs* world) {
    zox_system(
        GenerateNameSystem,
        zoxp_mainthread,
        [in] names.GenerateName,
        [in] core.Seed,
        [out] core.ZoxName
    );
}
