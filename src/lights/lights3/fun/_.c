#include "logs.c"
#include "prefab.c"
#include "fetch.c"
#include "nearby.c"

byte zox_chunk_mesh_lighting_busy(ecs* world, entity e) {
    if (disable_lights) {
        return 0;
    }
    return zox_has(e, BuildMeshColors); // ||
        // !zox_has(e, MeshColorsBuilt);
}

byte zox_chunk_lighting_busy(ecs* world, entity e) {
    if (disable_lights) {
        return 0;
    }
    return
        (zox_has(e, LightNodeDirty) && zox_getv(e, LightNodeDirty)) ||
        zox_get(e, SunlightQueue)->count ||
        zox_get(e, LightQueue)->count ||
        zox_get(e, DarkQueue)->count;
}