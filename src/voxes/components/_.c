zox_tag(Vox);
zoxc_byte(VoxType);
zoxc_byte(RubbleHeight);
zoxc_byte(GenerateVox);
zoxc_byte(CloneVox);
zoxc_byte(ChunkLod);    // we using this??!?!
zoxc_byte(VoxUniqueColors);
zoxc_int(RubbleCount);
zoxc_byte(VRegions);
zoxc_entity(CloneVoxLink);
zoxc_float(VoxColorRange);

void define_components_voxes(ecs *world) {
    zoxd_tag(Vox);
    zoxd_byte(VoxType);
    zoxd_byte(RubbleHeight);
    zoxd_byte(GenerateVox);
    zoxd_byte(ChunkLod);
    zoxd_byte(CloneVox);
    zoxd_byte(VoxUniqueColors);
    zoxd_byte(VRegions);
    zox_define_component_int(RubbleCount);
    zoxd_entity(CloneVoxLink);
    zoxd_float(VoxColorRange);
}