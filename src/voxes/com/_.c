// Voxes
zox_tag(Vox);
zoxc_entity(VoxLink);
zoxc_byte(ChunkLod);    // we using this??!?!
zoxc_hashmap(ChunkLinks, int3)
zoxc_hashmap(Chunk2Links, int2)

// operations
zoxc_byte(CloneVox);
zoxc_entity(CloneVoxLink);
zoxc_state(CombineVox);
zoxc_entitiesw(CombineList);
zoxc_arrayd(CombinePositions, byte3);

// TODO: Move to Shapes or something
zoxc_byte(VoxType);
zoxc_byte(RubbleHeight);
zoxc_byte(VoxUniqueColors);
zoxc_int(RubbleCount);
zoxc_byte(VRegions);
zoxc_float(VoxColorRange);

void define_components_voxes(ecs *world) {
    zoxd_tag(Vox);
    zoxd_entity(VoxLink);
    zoxd_byte(ChunkLod);
    zoxd_hashmap(ChunkLinks);
    zoxd_hashmap(Chunk2Links);

    // operations
    zoxd_byte(CloneVox);
    zoxd_state(CombineVox);
    zoxd_entity(CloneVoxLink);
    zoxd_entitiesw(CombineList);
    zoxd_arrayd(CombinePositions);

    // TODO: Move to Shapes or something
    zoxd_byte(VoxType);
    zoxd_byte(RubbleHeight);
    zoxd_byte(VoxUniqueColors);
    zoxd_byte(VRegions);
    zoxd_int(RubbleCount);
    zoxd_float(VoxColorRange);
}
