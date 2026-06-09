#include "colors.c"
#include "blocks.c"
realm_clear_system(BiomeLinks);

void define_systems_biomes(ecs *world) {
    realm_clear_systemd(biomes, BiomeLinks);
    zox_system(
        BiomeBlocksDirtySystem,
        EcsPostUpdate,
        [in] core.Generate,
        [in] realms.RealmLink,
        // [in] blocks.BlockLinks,
        [none] biomes.Biome
    );
}
