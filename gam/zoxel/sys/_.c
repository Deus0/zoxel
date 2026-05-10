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

zox_declare_system_state_event(RealmTilemaps, GenerateRealm, zox_generate_realm_tilemaps, spawn_realm_tilemaps);
zox_declare_system_state_event(RealmAchievements, GenerateRealm, zox_generate_realm_achievements, spawn_realm_achievements);

void define_systems_zoxel(ecs *world) {
    zox_system_1(
        BiomesRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] core.Seed,
        [out] biomes.BiomeLinks,
        [out] colorz.Colors,
        [none] realms.Realm
    );
    zox_system_1(
        ModelsRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] colorz.Colors,
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
        [in] rendering.ModelLinks,
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
        BlocksRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [in] colorz.Colors,
        [in] rendering.ModelLinks,
        [out] blocks.BlockLinks,
        [out] blocks.BlocksDirty,
        [none] realms.Realm
    );

    zox_define_system_state_event_1(RealmTilemaps, zoxp_mainthread, realms.GenerateRealm, [none] realms.Realm);
    zox_define_system_state_event_1(RealmAchievements, zoxp_mainthread, realms.GenerateRealm, [none] realms.Realm);
}
