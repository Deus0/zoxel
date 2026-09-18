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
        StatsRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        FoodRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        SkillsRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        QuestsRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] characters.CharacterLinks,
        [none] realms.Realm
    );
    zox_system_1(
        DialogueRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        MusicRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        AchievementRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    zox_system_1(
        BiomesRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] biomes.BiomeLinks,
        [none] realms.Realm
    );
    zox_system_1(
        BlocksRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] blocks.BlocksDirty,
        [none] realms.Realm
    );
    zox_system_1(
        BiomeBlocksSystem,
        zoxp_spawn,
        [in] core.Generate,
        [in] core.Seed,
        [out] biomes.BiomeSkyColor,
        [none] biomes.Biome
    );
    // NOTE: Blocks must be set before items spawn if this is the case!
    // TODO: Spawn Block Items when the Realm BlocksDirty is flagged
    zox_system_1(
        ItemsRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] blocks.BlockLinks,
        [none] realms.Realm
    );
    zox_system_1(
        BodysRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [none] realms.Realm
    );
    zox_system_1(
        EquipsRealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [none] realms.Realm
    );
    // Confirmed Crashes
    // Worlds
    zox_system_1(
        Character3RealmSpawnSystem,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] characters.CharacterLinks,
        [out] characters3.CharactersChanceMax,
        [none] realms.Realm
    );
}
