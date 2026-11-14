// Lights for gpu meshing (chunks3/fun/gpu_mesh.c, doc/tec/gpu_meshing.md)
//  - a light octree flattened with max values: a coarse read is its leaves' brightest, as getv_nearby_max_LightNode
//  - tst gpu_mesh_lights checks the gpu's corner lights against smooth_face_lights

// returns the node count, the nodes malloced into *out (free them), 0 on failure
static inline uint flatten_LightNode(const LightNode* root, uint** out) {
    return flatten_octree(root, sizeof(LightNode), octree_type_offset_LightNode, 1, out);
}

// gpu meshing's lighting from smooth lighting's: darklight, sunlight and which voxels stop light
//  - solidity: zox_light_solidity_length bytes, voxel v stops light when solidity[v - 1] (realm_light_solidity)
static inline GpuMeshLighting gpu_mesh_lighting(const byte* solidity) {
    GpuMeshLighting lighting = { .darklight = darklight, .sunlight = sunlight };
    for (int i = 0; i < zox_light_solidity_length; i++) {
        if (solidity[i]) {
            lighting.solid[i >> 5] |= 1u << (i & 31);
        }
    }
    return lighting;
}
