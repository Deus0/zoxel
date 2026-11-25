// a voxel's face into a mesh: its 6 indices (face_indices, offset by the vertices so far) and its 4 corners (corners,
//  a voxel's: voxel_face_vertices_n[direction]) scaled and moved to origin, the voxel's corner in the mesh, in order
//  - the one face writer of the cpu meshers (doc/plans/chunks_refactor.md phase 1): textured (sys/build_grid.c) adds
//    each corner's uv after, colored (sys/build_colored.c) each corner's colour; skeleton weights read a mesh as 4
//    vertices a face, with the voxel behind it (bones/sys/generate.c), so keep the order
static inline void mesh_add_face(int_array_d* indices, float3_array_d* vertices, const int* face_indices,
    const float3* corners, float3 origin, float scale)
{
    const int start = vertices->size;
    for (byte i = 0; i < 6; i++) {
        int_array_d_add(indices, start + face_indices[i]);
    }
    for (byte i = 0; i < 4; i++) {
        float3_array_d_add(vertices, (float3) { origin.x + corners[i].x * scale, origin.y + corners[i].y * scale,
            origin.z + corners[i].z * scale });
    }
}

// a colored model's kinds table for build_voxel_mesh_material: a voxel value's (its palette entry + 1) material, from
//  its MaterialIDs (NULL: every voxel solid); returns 1 if any voxel is see through (another mesh to build)
static inline byte vox_material_table(const MaterialIDs* ids, byte* table) {
    memset(table, zox_block_material_solid, 256);
    table[0] = zox_block_material_none;
    byte other = 0;
    for (int i = 0; ids && i < ids->length && i < 255; i++) {
        table[i + 1] = ids->value[i];
        other |= ids->value[i] != zox_block_material_solid;
    }
    return other;
}
