#include "colors.c"
#include "blocks.c"
#include "blocks_dirty.c"
realm_clear_system(BiomeLinks);

void define_systems_biomes(ecs *world) {
    realm_clear_systemd(biomes, BiomeLinks);
    /*zox_system(
        BiomeColorsSystem,
        EcsPreUpdate,
        [in] core.Generate,
        [in] core.Seed,
        [out] colorz.Colors,
        [none] biomes.Biome
    );*/
    zox_system_1(
        BiomeBlocksDirtySystem,
        EcsPostUpdate,
        [in] core.Generate,
        [in] realms.RealmLink,
        [in] blocks.BlockLinks,
        [none] biomes.Biome
    );
}
