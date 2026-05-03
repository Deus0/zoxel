#include "biomes.c"
#include "models.c"
#include "stats.c"
#include "skills.c"
#include "items_blocks.c"
#include "items_bodys.c"
#include "items_equips.c"
#include "quests.c"
#include "dialogues.c"
#include "blocks.c"
#include "characters.c"
#include "music.c"
#include "achievements.c"

zox_declare_system_state_event(RealmBlocks, GenerateRealm, zox_generate_realm_blocks, spawn_realm_blocks);
zox_declare_system_state_event(RealmTilemaps, GenerateRealm, zox_generate_realm_tilemaps, spawn_realm_tilemaps);
zox_declare_system_state_event(RealmAchievements, GenerateRealm, zox_generate_realm_achievements, spawn_realm_achievements);

void define_systems_zoxel(ecs *world) {

    zox_system_1(
        BiomesRealmSpawnSystem,
        EcsOnUpdate,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] biomes.BiomeLinks,
        [out] colorz.Colors,
        [none] realms.Realm
    );

    zox_system_1(
        ModelsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] colorz.Colors,
        [out] rendering.ModelLinks,
        [out] nodes.NodegraphLinks,
        [none] realms.Realm
    );

    zox_system_1(
        StatsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [out] stats.StatLinks,
        [none] realms.Realm
    );

    zox_system_1(
        SkillsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] stats.StatLinks,
        [out] skills.SkillLinks,
        [none] realms.Realm
    );

    zox_system_1(
        ItemsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] blocks.BlockLinks,
        [out] items.ItemLinks,
        [none] realms.Realm
    );

    zox_system_1(
        BodysRealmSpawnSystem,
        EcsOnUpdate,
        [in] realms.GenerateRealm,
        [out] rendering.ModelLinks,
        [out] items.ItemLinks,
        [out] nodes.NodegraphLinks,
        [none] realms.Realm
    );

    zox_system_1(
        EquipsRealmSpawnSystem,
        EcsOnUpdate,
        [in] realms.GenerateRealm,
        [in] rendering.ModelLinks,
        [out] items.ItemLinks,
        [none] realms.Realm
    );

    zox_system_1(
        QuestsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] characters.CharacterLinks,
        [out] quests.QuestLinks,
        [none] realms.Realm
    );

    zox_system_1(
        DialogueRealmSpawnSystem,
        EcsOnLoad,
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
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] rendering.ModelLinks,
        [out] characters.CharacterLinks,
        [out] characters3.CharactersChanceMax,
        [none] realms.Realm
    );

    zox_define_system_state_event_1(RealmBlocks, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
    zox_define_system_state_event_1(RealmTilemaps, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
    zox_define_system_state_event_1(RealmAchievements, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
}
