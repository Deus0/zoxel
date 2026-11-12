#include "tilemap.c"
#include "spawn.c"
#include "uvs.c"
#include "indexes.c"
zoxc_system_dirty(InsideBlock);

void zox_systems_blocks(ecs* world) {
    zoxd_system_dirty(InsideBlock);
    zox_system(
        realm_tilemap_system,
        zoxp_update,
        [in] blocks.BlockLinks,
        [none] realms.Realm,
        [none] blocks.BlocksTilemapUpdate,
    );
    zox_system_1(
        tilemap_realm_spawn_system,
        zoxp_spawn,
        [none] realms.Realm
    );
    zox_system(
        tilemap_uv_system,
        zoxp_update,
        // [in] textures.TextureLinks,
        [in] textures.TilemapSize,
        [in] rendering.TextureSize,
        [out] textures.GenerateTexture,
        [out] textures.TilemapUVs,
        [none] textures.Tilemap
    );
    zox_system(
        realm_blocks_dirty_system,
        zoxp_update,
        [out] blocks.BlockLinks,
        [none] realms.Realm,
        [none] blocks.BlocksDirty,
    );
}
