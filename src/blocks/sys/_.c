#include "tilemap.c"

void define_systems_blocks(ecs* world) {
    zox_system_1(
        TilemapRealmSpawnSystem,
        EcsOnUpdate,
        [out] textures.TilemapLink,
        [none] realms.Realm
    );
    zox_system(
        RealmTilemapSystem,
        EcsOnUpdate,
        [out] blocks.BlocksDirty,
        [in] blocks.BlockLinks,
        [in] textures.TilemapLink,
        [none] realms.Realm
    );
}
