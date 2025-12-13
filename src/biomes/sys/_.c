#include "realm.c"
#include "colors.c"
#include "blocks.c"
#include "blocks_dirty.c"
zox_declare_system_state_event(RealmBiomes, GenerateRealm, zox_generate_realm_biomes, spawn_realm_biomes)

void define_systems_biomes(ecs *world) {
    zox_define_system_state_event_1(RealmBiomes, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
    zox_system(
        BiomeColorsSystem,
        EcsPreUpdate,
        [in] generic.Generate,
        [in] generic.Seed,
        [out] colorz.Colors,
        [none] biomes.Biome
    );
    zox_system_1(
        BiomeBlocksSystem,
        EcsOnUpdate,
        [in] generic.Generate,
        [in] colorz.Colors,
        [out] blocks.BlockLinks,
        [none] biomes.Biome
    );
    zox_system_1(
        BiomeBlocksDirtySystem,
        EcsPostUpdate,
        [in] generic.Generate,
        [in] realms.RealmLink,
        [in] blocks.BlockLinks,
        [none] biomes.Biome
    );
}