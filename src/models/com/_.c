zox_tag(Model);
zox_tag(ModelGroup);
zox_tag(HueComplement);    // a colors node: its colour the complement of the model's first (seeded hue, contrast)
zox_tag(ModelCharacter);
zox_tag(BakeModel);
zox_tag(ModelRun);
zox_tag(SameXZ);
zox_tag(SameXY);
zoxc_byte3(ModelSize);
zoxc_float6(ModelRatio);
zoxc_byte(EyeRidge);
zoxc_byte3(EyeSize);
zoxc_byte(PlaceType);
zoxc_byte(NodeVoxel);
zoxc_byte(NodeColors);
zoxc_ushort(WfcRules);
zoxc_float3(ColorMutation);   // a mutate node's range: hue turn (degrees), saturation added, value change (a share)        // a wfc node's rules (wfc_rule_*, fun/wfc.c)
// voxes
zoxc_byte(VoxType);
zoxc_byte(RubbleHeight);
zoxc_byte(VoxUniqueColors);
zoxc_int(RubbleCount);
zoxc_byte(VRegions);
zoxc_float(VoxColorRange);

void zox_components_models(ecs* world) {
    zoxd_tag(Model);
    zoxd_tag(ModelGroup);
    zoxd_tag(HueComplement);
    zoxd_tag(ModelCharacter);
    zoxd_tag(BakeModel);
    zoxd_tag(ModelRun);
    zoxd_tag(SameXZ);
    zoxd_tag(SameXY);
    zoxd_byte3(ModelSize);
    zoxd_float6(ModelRatio);
    zoxd_byte(EyeRidge);
    zoxd_byte3(EyeSize);
    zoxd_byte(PlaceType);
    zoxd_byte(NodeVoxel);
    zoxd_byte(NodeColors);
    zoxd_ushort(WfcRules);
    zoxd_float3(ColorMutation);
    // TODO: Voxes
    zoxd_byte(VoxType);
    zoxd_byte(RubbleHeight);
    zoxd_byte(VoxUniqueColors);
    zoxd_byte(VRegions);
    zoxd_int(RubbleCount);
    zoxd_float(VoxColorRange);
}
