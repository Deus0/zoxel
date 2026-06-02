#include "tilemap.c"
#include "spawn.c"
#include "uvs.c"

void define_systems_blocks(ecs* world) {
    zox_system(
        RealmTilemapSystem,
        EcsOnUpdate,
        [in] blocks.BlocksDirty,
        [in] blocks.BlockLinks,
        [in] textures.TilemapLink,
        [none] realms.Realm
    );
    zox_system_1(
        TilemapRealmSpawnSystem,
        zoxp_mainthread,
        [out] textures.TilemapLink,
        [none] realms.Realm
    );
    zox_system(
        TilemapUVSystem,
        zox_pip_texture_generation,
        [in] rendering.TextureDirty,
        [in] textures.TextureLinks,
        [in] textures.TilemapSize,
        [in] rendering.TextureSize,
        [in] realms.RealmLink,
        [out] textures.TilemapUVs,
        [none] textures.Tilemap
    );
}
