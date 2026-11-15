// chunk materials: a terrain chunk's faces are built and meshed per block material (blocks/dat/model_types.c)
//  - chunk > a ChunkMaterial child per material its voxels have (its BlockMaterial, SidesOctree, ActiveMesh)
//    > a ChunkMesh child per lod
//  - spawned when the chunk's voxels change (chunk_mesh_spawn_system), kept while the chunk lives
#define zox_chunk_meshes_max 32     // a chunk's lod meshes, over all its materials

// a chunk's materials, into an array of zox_block_materials
static inline uint get_chunk_materials(ecs* world, entity chunk, entity* materials) {
    return zox_get_children_by_id(world, chunk, materials, zox_block_materials, zox_id(ChunkMaterial));
}

// a chunk's material for a block material, 0 if it has none
static inline entity get_chunk_material(ecs* world, entity chunk, byte material) {
    entity materials[zox_block_materials];
    const uint count = get_chunk_materials(world, chunk, materials);
    for (uint i = 0; i < count; i++) {
        if (zox_getv(materials[i], BlockMaterial) == material) {
            return materials[i];
        }
    }
    return 0;
}

// a chunk's meshes, every material's and lod's, into an array of zox_chunk_meshes_max
static inline uint get_chunk_meshes(ecs* world, entity chunk, entity* meshes) {
    entity materials[zox_block_materials];
    const uint materials_count = get_chunk_materials(world, chunk, materials);
    uint count = 0;
    for (uint i = 0; i < materials_count; i++) {
        count += zox_get_children_by_id(world, materials[i], &meshes[count], zox_chunk_meshes_max - count, zox_id(ChunkMesh));
    }
    return count;
}

// a chunk mesh's chunk: its material's parent
static inline entity get_chunk_mesh_chunk(ecs* world, entity mesh) {
    const entity material = zox_get_parent(world, mesh);
    return zox_valid(material) ? zox_get_parent(world, material) : 0;
}

// a material's mesh at a depth, 0 if it has none
static inline entity chunk_mesh_at_depth(ecs* world, entity material, byte depth) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
    for (uint i = 0; i < count; i++) {
        if (zox_getv(meshes[i], RenderDepth) == depth) {
            return meshes[i];
        }
    }
    return 0;
}

// is one of the chunk's shown meshes still building or uploading
static inline byte chunk_meshes_loading(ecs* world, entity chunk) {
    entity materials[zox_block_materials];
    const uint count = get_chunk_materials(world, chunk, materials);
    for (uint i = 0; i < count; i++) {
        const entity mesh = zox_get_link(world, materials[i], ActiveMesh);
        if (zox_valid(mesh) && (zox_has(mesh, BuildMesh) || zox_has(mesh, MeshDirty))) {
            return 1;
        }
    }
    return 0;
}

// stops the chunk's shown meshes building, as its depth changed and new ones take over
static inline void chunk_meshes_stop_building(ecs* world, entity chunk) {
    entity materials[zox_block_materials];
    const uint count = get_chunk_materials(world, chunk, materials);
    for (uint i = 0; i < count; i++) {
        const entity mesh = zox_get_link(world, materials[i], ActiveMesh);
        if (zox_valid(mesh) && !zox_has(mesh, BuildDisabled)) {
            zox_add(mesh, BuildDisabled);
        }
    }
}

// the faces drawn at a voxel, of any of the chunk's materials (a bit per direction, as SidesOctree)
static inline byte get_chunk_sides(ecs* world, entity chunk, byte depth, byte3 position) {
    entity materials[zox_block_materials];
    const uint count = get_chunk_materials(world, chunk, materials);
    byte sides = 0;
    for (uint i = 0; i < count; i++) {
        sides |= getv_SidesOctree(zox_get(materials[i], SidesOctree), depth, position);
    }
    return sides;
}

// the block materials in a voxel octree, a bit each (1 << material)
static inline byte get_node_materials(const VoxelNode* node, const byte* table) {
    if (!has_children_VoxelNode(node)) {
        return 1 << table[node->value];
    }
    const VoxelNode* kids = get_children_VoxelNode(node);
    byte mask = 0;
    for (byte i = 0; i < 8 && kids; i++) {
        mask |= get_node_materials(&kids[i], table);
    }
    return mask;
}
