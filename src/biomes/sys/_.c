#include "colors.c"
#include "blocks.c"
realm_clear_system(BiomeLinks);

void define_systems_biomes(ecs *world) {
    realm_clear_systemd(biomes, BiomeLinks);
    zox_system(
        BiomeBlocksDirtySystem,
        zoxp_update,
        [in] core.Generate,
        [in] realms.RealmLink,
        [none] biomes.Biome
    );
}
