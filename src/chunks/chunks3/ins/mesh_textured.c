// a chunk's material: the sides and meshes of its blocks of one block material, drawn with a gpu material
entity spawn_chunk_material(
    ecs* world,
    entity chunk,
    byte block_material,
    entity material)
{
    entity e = zox_ins(world, prefab_chunk_material);
    zox_set_unique_name(e, "chunk_material");
    zox_setv(e, BlockMaterial, block_material);
    zox_link(world, e, MaterialLink, material);
    initialize_sides_lock(world, e);
    zox_set_parent_non_fragment(world, e, chunk);
    return e;
}

entity spawn_chunk_mesh_textured(
    ecs* world,
    entity parent,
    float4x4 matrix,
    byte depth,
    byte render_disabled,
    entity tilemap,
    byte block_material)
{
    // solid blocks meshed on the gpu when it's on (water stays on the cpu, drawn blended)
    const byte gpu = chunk_material_gpu(block_material);
    entity e2 = zox_ins(world, gpu ? prefab_chunk_mesh_gpu : prefab_chunk_mesh_textured);
    zox_set_unique_name(e2, "chunk_mesh");
    zox_setv(e2, TransformMatrix, matrix);
    zox_setv(e2, RenderDepth, depth);
    zox_setv(e2, RenderDisabled, render_disabled);
    zox_link(world, e2, MaterialLink, tilemap);
    if (block_material == zox_block_material_liquid) {
        zox_add(e2, TransparentMesh);
        // its own texture: the realm's liquid block's (not on the tilemap)
        const entity realm = zox_get_link(world, tilemap, RealmLink);
        const BlockLinks* blocks = zox_valid(realm) && zox_has(realm, BlockLinks) ? zox_get(realm, BlockLinks) : NULL;
        for (int i = 0; blocks && i < blocks->length; i++) {
            const entity block = blocks->value[i];
            if (zox_valid(block) && zox_has(block, BlockMaterial) && zox_getv(block, BlockMaterial) == zox_block_material_liquid) {
                const entity texture = zox_get_child_by_id(world, block, zox_id(Texture));
                if (zox_valid(texture)) {
                    zox_link(world, e2, MeshTextureLink, texture);
                }
                break;
            }
        }
    }
    zox_set_parent_non_fragment(world, e2, parent);
    return e2;
}
