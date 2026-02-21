#include "player.c"
#include "character.c"
#include "realm.c"

void define_systems_bodys(ecs* world) {

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
        CharacterBodySpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] bodys.BodyLinks,
        [out] bodys.BodyDirty,
        [none] players.PlayerCharacter
    );

    zox_system(
        PlayerBodySpawnSystem,
        EcsOnUpdate,
        [in] bodys.BodyDirty,
        [in] bodys.BodyLinks,
        [in] rendering.RenderDepth,
        [none] players.PlayerCharacter
    );
}
