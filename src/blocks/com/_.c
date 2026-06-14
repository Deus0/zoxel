zox_tag(Voxel);
zox_tag(Block);
zox_tag(BlockVox);
zox_tag(BlockDungeon);
zox_tag(BlockInvinsible);
zox_tag(BlockCanGroup);
zoxc_byte(BlockLightPass);
zoxc_byte(BlockModel);
zoxc_byte(BlockCollider); // zox_block_air | zox_block_solid
zoxc_state(BlocksDirty);
zoxc_byte(VoxBakeSide);
zoxc_byte(BlockVoxOffset);
zoxc_byte(BlockIndex);      // NOTE: This includes Air in the index
zoxc_int3(BlockPosition);
zoxc_int3(BlockSize);
zoxc_int2(BlockPosition2);
zoxc_int2(BlockSize2);
zoxc_byte3(VoxelLocalPosition);
zoxc_float(BlockScale);
zoxc_entity(BlockLink);
zoxc_entity(BlockPrefabLink);
zoxc_entities(BlockLinks)
zoxc_entity(BlockManagerLink);
zoxc_byte(InsideBlock);
zoxc_int3(InsideBlockPosition);
zoxc_state(InsideBlockDirty);
zoxc_double(InsideBlockTime);
zox_tag(BlockSoundTrigger);
zoxc_entity(BlockSound);        // Links Block to a Sound
zoxc_float2(BlockHealth);

entity get_block_link(ecs* world, entity e) {
    return zox_getv(e, BlockLink);
}

void zox_define_components_blocks(ecs *world) {
    zoxd_tag(Voxel);
    zoxd_tag(Block);
    zoxd_tag(BlockVox);
    zoxd_tag(BlockDungeon);
    zoxd_tag(BlockInvinsible);
    zoxd_tag(BlockCanGroup);
    zoxd_byte(BlockLightPass);
    zoxd_byte(BlockModel);
    zoxd_byte(BlockCollider);
    zoxd_state(BlocksDirty);
    zoxd_byte(VoxBakeSide);
    zoxd_byte(BlockVoxOffset);
    zoxd_byte(BlockIndex);
    zoxd_int3(BlockPosition);
    zoxd_int3(BlockSize);
    zoxd_int2(BlockPosition2);
    zoxd_int2(BlockSize2);
    zoxd_byte3(VoxelLocalPosition);
    zoxd_float(BlockScale);
    zoxd_entity(BlockLink);
    zoxd_entity(BlockPrefabLink);
    zoxd_entities(BlockLinks);
    zoxd_entity(BlockManagerLink);
    zoxd_byte(InsideBlock);
    zoxd_int3(InsideBlockPosition);
    zoxd_state(InsideBlockDirty);
    zoxd_double(InsideBlockTime);
    zoxd_tag(BlockSoundTrigger);
    zoxd_entity(BlockSound);
    zoxd_float2(BlockHealth);
}
