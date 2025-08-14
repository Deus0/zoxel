void prefab_add_lights3(
    ecs* world,
    entity e,
    byte depth
) {
    zox_prefab_set(e, LightNode, { 0 });
    zox_prefab_set(e, LightNodeDepth, { depth });
    zox_prefab_set(e, LightNodeDirty, { zox_dirty_none });
    zox_prefab_set(e, LightNodeQueue, { });
}

void fetch_neightbor_voxel_nodes(
    ecs* world,
    const ChunkNeighbors* neighbors,
    const VoxelNode** nodes
) {
    for (int i = 0; i < 6; i++) {
        const entity e = neighbors->value[i];
        nodes[i] = zox_valid(e) && !zox_has(e, VoxelNode) ? zox_gett(e, VoxelNode) : NULL;
    }
}

void fetch_neightbor_light_nodes(
    ecs* world,
    const ChunkNeighbors* neighbors,
    const LightNode** nodes
) {
    for (int i = 0; i < 6; i++) {
        const entity e = neighbors->value[i];
        nodes[i] = zox_valid(e) && !zox_has(e, LightNode) ? zox_gett(e, LightNode) : NULL;
    }
}

#include "propogate.c"