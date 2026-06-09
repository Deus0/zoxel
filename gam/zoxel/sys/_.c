#include "models.c"
#include "stats.c"
#include "skills.c"
#include "items_blocks.c"
#include "body_parts.c"
#include "equipment.c"
#include "quests.c"
#include "dialogues.c"
#include "blocks.c"
#include "characters.c"
#include "music.c"
#include "achievements.c"
#include "biomes.c"
#include "biome_blocks.c"

void define_systems_zoxel(ecs *world) {
    zox_system_1(
        ModelsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] rendering.ModelLinks,
        [out] nodes.NodegraphLinks,
        [none] realms.Realm
    );
    zox_system_1(
        StatsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] stats.StatLinks,
        [none] realms.Realm
    );
    zox_system_1(
        SkillsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] stats.StatLinks,
        [out] skills.SkillLinks,
        [none] realms.Realm
    );
    // NOTE: Blocks must be set before items spawn if this is the case!
    // TODO: Spawn Block Items when the Realm BlocksDirty is flagged
    zox_system_1(
        ItemsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] blocks.BlockLinks,
        [out] items.ItemLinks,
        [none] realms.Realm
    );
    zox_system_1(
        BodysRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] rendering.ModelLinks,
        [out] items.ItemLinks,
        [out] nodes.NodegraphLinks,
        [none] realms.Realm
    );
    zox_system_1(
        EquipsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        // [out] rendering.ModelLinks,
        [out] items.ItemLinks,
        [none] realms.Realm
    );
    zox_system_1(
        QuestsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] characters.CharacterLinks,
        [out] quests.QuestLinks,
        [none] realms.Realm
    );
    zox_system_1(
        DialogueRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        // [in] blocks.BlockLinks,
        [out] dialogues.DialoguetreeLinks,
        [none] realms.Realm
    );
    zox_system_1(
        MusicRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] musics.PlaylistLinks,
        [none] realms.Realm
    );
    zox_system_1(
        Character3RealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] rendering.ModelLinks,
        [out] characters.CharacterLinks,
        [out] characters3.CharactersChanceMax,
        [none] realms.Realm
    );
    zox_system_1(
        AchievementRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] achievements.AchievementLinks,
        [none] realms.Realm
    );
    zox_system_1(
        BiomesRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] biomes.BiomeLinks,
        [none] realms.Realm
    );
    zox_system_1(
        BlocksRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] blocks.BlocksDirty,
        [none] realms.Realm
    );
    zox_system_1(
        BiomeBlocksSystem,
        zoxp_mainthread,
        [in] core.Generate,
        [in] core.Seed,
        [out] colorz.Colors,
        [none] biomes.Biome
    );
}
