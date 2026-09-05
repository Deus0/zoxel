#include "tilemap.c"
#include "spawn.c"
#include "uvs.c"
#include "indexes.c"

void zox_define_systems_blocks(ecs* world) {
    zox_system(
        RealmTilemapSystem,
        zoxp_update,
        [in] blocks.BlockLinks,
        [in] textures.TilemapLink,
        [out] blocks.BlocksDirty,
        [none] realms.Realm
    );
    zox_system_1(
        TilemapRealmSpawnSystem,
        zoxp_spawn,
        [out] textures.TilemapLink,
        [none] realms.Realm
    );
    zox_system(
        TilemapUVSystem,
        zoxp_update,
        [in] textures.TextureLinks,
        [in] textures.TilemapSize,
        [in] rendering.TextureSize,
        [in] realms.RealmLink,
        [out] textures.GenerateTexture,
        [out] textures.TilemapUVs,
        [none] textures.Tilemap
    );
    zox_system(
        RealmBlocksDirtySystem,
        zoxp_update,
        [out] blocks.BlocksDirty,
        [out] blocks.BlockLinks,
        [none] realms.Realm
    );
}
