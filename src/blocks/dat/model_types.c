#define zox_block_air 0
#define zox_block_solid 1
#define zox_block_vox 2
#define zox_block_open 3     // a collider: solid unless it's on (its chunk's ActiveOctree: a door, its filler)

// block materials: a chunk's cube faces are built and meshed per material (chunks3 ChunkMaterial)
//  - a face draws against air, a block with no material, or a see through material that isn't its own
//  - so ground draws against water, water only against air
#define zox_block_material_none 0       // no cube faces: air, vox models
#define zox_block_material_solid 1      // opaque: hides every face against it
#define zox_block_material_liquid 2     // see through: hides only its own faces, drawn blended (water)
#define zox_block_material_oil 3        // a liquid as water is, its own mesh and texture
#define zox_block_materials 4

// a see through liquid, drawn blended with its own texture: water, oil
static inline byte block_material_liquid(byte material) {
    return material == zox_block_material_liquid || material == zox_block_material_oil;
}
