zox_tag(Voxel);
zox_tag(Block);
zox_tag(BlockBaked);
zox_tag(BlockVox);
zox_tag(BlockDungeon);
zox_tag(BlockInvinsible);
zox_tag(BlockCanGroup);
zox_tag(SingleTextureBlock);
zoxc_byte(BlockLightPass);   // 1 light passes it (water, minivoxes), 0 it stops light
#define block_light_pass_open 2  //  passes only while its block data is open (a door and its filler, vodes/doors)
#define zox_water_level_source 8
zoxc_byte(BlockWaterLevel); // water's depth in eighths of a block (terrain/sys/water_flow.c): a source 8, flowing 7 to 1,
                            //  a flowing block's top is drawn at its level (chunks3 build_textured.c)
zoxc_byte(BlockLight);      // light a block gives off, 0 none (glow stone), it floods out of its sides
zoxc_byte3(BlockLightOffset); // where from, blocks from its base (a street lamp's head), turned with it; none: its base
zox_tag(BlockTableDirty);   // a realm's block indices changed, its saved chunks remap (chunks3/io/sys/block_table.c)
zoxc_byte(BlockModel);
zoxc_byte(BlockCollider); // zox_block_air | zox_block_solid
zoxc_byte(BlockMaterial);  // zox_block_material_*: what a cube block's faces draw with, and a ChunkMaterial's blocks
zox_tag(BlocksDirty);
zox_tag(BlocksTilemapUpdate);
zoxc_byte(VoxBakeSide);
zoxc_byte(BlockVoxOffset);
zoxc_byte(BlockIndex);      // NOTE: This includes Air in the index
zoxc_int3(BlockPosition);
zoxc_int3(BlockSize);
zoxc_int2(BlockPosition2);
zoxc_int2(BlockSize2);
zoxc_byte3(VoxelLocalPosition);
zoxc_float(BlockScale);
zox_tag(BlockLink);
zoxc_entity(BlockPrefabLink);
zoxc_entities(BlockLinks)
zox_tag(BlockManagerLink);
zoxc_byte(InsideBlock);
zoxc_int3(InsideBlockPosition);
zoxc_double(InsideBlockTime);
zox_tag(BlockSoundTrigger);
zoxc_entity(BlockSound);        // Links Block to a Sound
zoxc_float2(BlockHealth);
#include "queue.c"
zox_tag(InsideBlockDirty);    // entered a block (a non fragmenting event: observers, a cleanup removes it)

entity get_block_link(ecs* world, entity e) {
    return zox_get_link(world, e, BlockLink);
}

void zox_components_blocks(ecs *world) {
    zoxd_tag(Voxel);
    zoxd_tag(Block);
    zoxd_tag(BlockBaked);
    zoxd_tag(BlockVox);
    zoxd_tag(BlockDungeon);
    zoxd_tag(BlockInvinsible);
    zoxd_tag(BlockCanGroup);
    zoxd_tag(BlockSoundTrigger);
    zoxd_nf_tag(SingleTextureBlock);
    zoxd_byte(BlockLightPass);
    zoxd_byte(BlockWaterLevel);
    zoxd_byte(BlockLight);
    zoxd_byte3(BlockLightOffset);
    zoxd_tag(BlockTableDirty);
    zoxd_byte(BlockModel);
    zoxd_byte(BlockCollider);
    zoxd_byte(BlockMaterial);
    zoxd_byte(VoxBakeSide);
    zoxd_byte(BlockVoxOffset);
    zoxd_byte(BlockIndex);
    zoxd_byte(InsideBlock);
    zoxd_tag(BlocksDirty);
    zoxd_tag(BlocksTilemapUpdate);
    zoxd_byte3(VoxelLocalPosition);
    zoxd_int2(BlockPosition2);
    zoxd_int2(BlockSize2);
    zoxd_int3(BlockPosition);
    zoxd_int3(BlockSize);
    zoxd_int3(InsideBlockPosition);
    zoxd_float(BlockScale);
    zoxd_double(InsideBlockTime);
    zoxd_float2(BlockHealth);
    zoxd_nf_link(BlockLink);
    zoxd_entity(BlockPrefabLink);
    zoxd_nf_link(BlockManagerLink);
    zoxd_entity(BlockSound);
    zoxd_entities(BlockLinks);
    zoxd_queue(BlockDamageQueue);
    zoxd_nf_tag(InsideBlockDirty);
}
