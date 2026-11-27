// each voxel value's block material (zox_block_material_*), 0 for air and blocks without cube faces (vox models)
//  - table: 256 bytes, indexed by the voxel value; every block solid if the realm has none
static inline void get_block_materials(ecs* world, entity realm, byte* table) {
    memset(table, zox_block_material_solid, 256);
    table[0] = zox_block_material_none;
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        return;
    }
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    for (int i = 0; i < blocks->length && i < 255; i++) {
        const entity block = blocks->value[i];
        if (!zox_valid(block) || !zox_has(block, BlockModel)) {
            continue;
        }
        if (zox_getv(block, BlockModel) != zox_block_solid) {
            table[i + 1] = zox_block_material_none;
        } else if (zox_has(block, BlockMaterial)) {
            table[i + 1] = zox_getv(block, BlockMaterial);
        }
    }
}

// each voxel value's glow (a cube block's BlockGlow, 0 none); its faces' colour alpha (MeshColors)
static inline void get_block_glows(ecs* world, entity realm, byte* table) {
    memset(table, 0, 256);
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        return;
    }
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    for (int i = 0; i < blocks->length && i < 255; i++) {
        const entity block = blocks->value[i];
        if (zox_valid(block) && zox_has(block, BlockGlow)) {
            table[i + 1] = zox_getv(block, BlockGlow);
        }
    }
}

// does a neighbor of this material hide a face of that material
static inline byte block_material_hides(byte neighbor, byte material) {
    return neighbor == zox_block_material_solid || (neighbor && neighbor == material);
}
