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
zoxc_ushort(WfcRules);        // a wfc node's rules (wfc_rule_*, fun/wfc.c)
zoxc_float3(ColorMutation);   // a mutate node's range: hue turn (degrees), saturation added, value change (a share)
zoxc_float6(ColorHSV);        // a colors node's colour seeded in this range: hue (degrees), saturation, value (0 to 100)
// animations: a VoxAnimation (a model group) whose children are its frames (spawn_blueprint_animation)
zox_tag(VoxAnimation);        // a model group of an animation's frames, played in ModelFrame order
zoxc_byte(AnimationFps);      // a VoxAnimation's frames a second
zoxc_byte(ModelFrame);        // a frame's place in its VoxAnimation, its nodes read it (leds chase by it)
zox_tag(ModelActive);         // a frame of an animation shown while its block is on (rune lights on it)
zox_tag(ActiveModelLink);     // a block's model while it's on (its chunk's ActiveOctree), its ModelLink while off
zox_tag(ModelAnimated);       // a block (and its block voxes) whose models are VoxAnimations (vodes3 animate)
// voxes
zoxc_byte(VoxType);
zoxc_byte(RubbleHeight);
zoxc_byte(VoxUniqueColors);
zoxc_int(RubbleCount);
zoxc_byte(VRegions);
zoxc_float(VoxColorRange);

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingBlockRes);
zox_tag(SettingOutlines);
zox_tag(SettingTerrainRes);

void zox_components_models(ecs* world) {
    zoxd_tag(SettingBlockRes);
    zoxd_tag(SettingOutlines);
    zoxd_tag(SettingTerrainRes);

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
    zoxd_float6(ColorHSV);
    zoxd_tag(VoxAnimation);
    zoxd_byte(AnimationFps);
    zoxd_byte(ModelFrame);
    zoxd_tag(ModelActive);
    zoxd_nf_link(ActiveModelLink);
    zoxd_tag(ModelAnimated);
    // TODO: Voxes
    zoxd_byte(VoxType);
    zoxd_byte(RubbleHeight);
    zoxd_byte(VoxUniqueColors);
    zoxd_byte(VRegions);
    zoxd_int(RubbleCount);
    zoxd_float(VoxColorRange);
}
