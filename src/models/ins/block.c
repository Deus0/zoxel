entity spawn_block_vox(
    ecs *world,
    SpawnBlockVox data)
{
    zox_instance(data.prefab);
    zox_name("block_vox");
    zox_setv(e, BlockScale, data.scale);
    zox_setv(e, RenderDepth, data.render_depth);
    zox_setv(e, RenderDisabled, data.render_disabled);
    zox_setv(e, BlockIndex, data.block_index);
    zox_setv(e, CloneVox, 1);
    zox_setv(e, CloneVoxLink, data.vox);
    zox_setv(e, Position3D, data.positionf);
    return e;
}
