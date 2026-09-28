entity spawn_block_vox(
    ecs *world,
    entity prefab,
    entity block,
    entity vox,
    byte block_index,
    byte render_depth,
    byte render_disabled,
    float3 positionf,
    float scale)
{
    zox_instance(prefab);
    zox_name("block_vox");
    zox_setv(e, BlockScale, scale);
    zox_setv(e, RenderDepth, render_depth);
    zox_setv(e, RenderDisabled, render_disabled);
    zox_setv(e, BlockIndex, block_index);
    zox_setv(e, CloneVox, 1);
    zox_setv(e, CloneVoxLink, vox);
    zox_setv(e, Position3D, positionf);
    zox_link(world, e, BlockLink, block);
    return e;
}
