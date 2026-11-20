// Lights for gpu meshing (chunks3/fun/gpu_mesh.c, doc/plans/gpu_meshing.md)
//  - the shader reads smooth lighting's grid (fun/smooth_grid.c): a chunk's voxels and lights with a voxel of border
//      from its 26 neighbours, a coarse light its leaves' brightest, as getv_nearby_max_LightNode
//  - tst gpu_mesh_lights checks the gpu's corner lights against smooth_face_lights

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

// meshes a chunk on the gpu from its smooth grid (smooth_grid_fill at depth), lighting NULL leaves it unlit
static inline void gpu_mesh_smooth_grid(GpuMesher* mesher, const smooth_grid* grid, byte depth, const GpuMeshLighting* lighting) {
    uint present = 0, lights_present = 0;
    for (byte i = 0; i < 27; i++) {
        present |= (uint) grid->voxels_present[i] << i;
        lights_present |= (uint) grid->present[i] << i;
    }
    gpu_mesh_grid(mesher, grid->voxels, grid->lights, depth, present, lights_present, lighting);
}
