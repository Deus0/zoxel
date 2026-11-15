#define zox_block_air 0
#define zox_block_solid 1
#define zox_block_vox 2

// block materials: a chunk's cube faces are built and meshed per material (chunks3 ChunkMaterial)
//  - a face draws against air, a block with no material, or a see through material that isn't its own
//  - so ground draws against water, water only against air
#define zox_block_material_none 0       // no cube faces: air, vox models
#define zox_block_material_solid 1      // opaque: hides every face against it
#define zox_block_material_liquid 2     // see through: hides only its own faces, drawn blended
#define zox_block_materials 3
