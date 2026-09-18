entity spawn_chunk_mesh_textured(
    ecs* world,
    entity parent,
    float4x4 matrix,
    byte depth,
    byte render_disabled,
    entity tilemap)
{
    entity e2 = zox_ins(world, prefab_chunk_mesh_textured);
    zox_set_unique_name(e2, "chunk_mesh");
    zox_setv(e2, TransformMatrix, matrix);
    zox_setv(e2, RenderDepth, depth);
    zox_setv(e2, RenderDisabled, render_disabled);
    zox_link(world, e2, MaterialLink, tilemap);
    zox_set_parent_non_fragment(world, e2, parent);
    return e2;
}