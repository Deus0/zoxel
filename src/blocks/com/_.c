zox_tag(Voxel);
zox_tag(Block);
zox_tag(BlockVox);
zox_tag(BlockDungeon);
zox_tag(BlockInvinsible);
zoxc_byte(BlockModel);
zoxc_byte(BlockCollider); // zox_block_air | zox_block_solid
zoxc_state(BlocksDirty);
zoxc_byte(VoxBakeSide);
zoxc_byte(BlockVoxOffset);
zoxc_byte(BlockIndex);
zoxc_int3(VoxelPosition);
zoxc_byte3(VoxelLocalPosition);
zoxc_float(BlockScale);
zoxc_entity(BlockLink);
zoxc_entity(BlockPrefabLink);
zoxc_entities(VoxelLinks)

void define_components_blocks(ecs_world_t *world) {
    zoxd_tag(Voxel);
    zoxd_tag(Block);
    zoxd_tag(BlockVox);
    zoxd_tag(BlockDungeon);
    zoxd_tag(BlockInvinsible);
    zoxd_byte(BlockModel);
    zoxd_byte(BlockCollider);
    zoxd_state(BlocksDirty);
    zoxd_byte(VoxBakeSide);
    zoxd_byte(BlockVoxOffset);
    zoxd_byte(BlockIndex);
    zoxd_int3(VoxelPosition);
    zoxd_byte3(VoxelLocalPosition);
    zoxd_float(BlockScale);
    zoxd_entity(BlockLink);
    zoxd_entity(BlockPrefabLink);
    zoxd_entities(VoxelLinks);
}
