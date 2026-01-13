// Voxes
zox_tag(Vox);
zoxc_entity(VoxLink);
zoxc_byte(ChunkLod);    // we using this??!?!
zoxc_hashmap(ChunkLinks, int3)
zoxc_hashmap(Chunk2Links, int2)

// TODO: Move to Shapes or something
zoxc_byte(VoxType);
zoxc_byte(CloneVox);
zoxc_state(GenerateVox);
zoxc_byte(RubbleHeight);
zoxc_byte(VoxUniqueColors);
zoxc_int(RubbleCount);
zoxc_byte(VRegions);
zoxc_entity(CloneVoxLink);
zoxc_float(VoxColorRange);

void define_components_voxes(ecs *world) {
    zoxd_tag(Vox);
    zoxd_entity(VoxLink);
    zoxd_byte(ChunkLod);
    zox_define_hashmap_component(ChunkLinks);
    zox_define_hashmap_component(Chunk2Links);

    // TODO: Move to Shapes or something
    zoxd_byte(VoxType);
    zoxd_state(GenerateVox);
    zoxd_byte(CloneVox);
    zoxd_entity(CloneVoxLink);
    zoxd_byte(RubbleHeight);
    zoxd_byte(VoxUniqueColors);
    zoxd_byte(VRegions);
    zoxd_int(RubbleCount);
    zoxd_float(VoxColorRange);
}