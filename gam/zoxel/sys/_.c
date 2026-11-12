#include "models.c"
#include "stats.c"
#include "skills.c"
#include "items_blocks.c"
#include "body_parts.c"
#include "equipment.c"
#include "food.c"
#include "quests.c"
#include "dialogues.c"
#include "blocks.c"
#include "characters.c"
#include "music.c"
#include "achievements.c"
#include "biomes.c"
#include "biome_blocks.c"

void zox_systems_zoxel(ecs *world) {
    zox_system_1(
        stats_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm,
    );
    zox_system_1(
        food_generate_system,
        zoxp_spawn,
        [in] core.Seed,
        [in] realms.GenerateRealm,
        [none] realms.Realm,
    );
    zox_system_1(
        skills_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm,
    );
    zox_system_1(
        quests_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        dialogue_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        music_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        achievement_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        biomes_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] biomes.BiomeLinks,
        [none] realms.Realm
    );
    zox_system_1(
        blocks_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        // [out] blocks.BlocksDirty,
        [none] realms.Realm
    );
    zox_system_1(
        biome_blocks_system,
        zoxp_spawn,
        [in] core.Generate,
        [in] core.Seed,
        [out] biomes.BiomeSkyColor,
        [none] biomes.Biome
    );
    // NOTE: Blocks must be set before items spawn if this is the case!
    // TODO: Spawn Block Items when the Realm BlocksDirty is flagged
    zox_system_1(
        items_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] blocks.BlockLinks,
        [none] realms.Realm
    );
    zox_system_1(
        bodys_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [none] realms.Realm
    );
    zox_system_1(
        equips_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    // Confirmed Crashes
    // Worlds
    zox_system_1(
        character3_realm_spawn_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] characters3.CharactersChanceMax,
        [none] realms.Realm
    );
}
