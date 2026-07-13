zox_tag(Model);
zox_tag(ModelGroup);
zox_tag(ModelCharacter);
zoxc_byte(EyeRidge);
zoxc_byte3(EyeSize);
zoxc_byte(PlaceType);
zoxc_byte(NodeVoxel);
zoxc_byte(NodeColors);

void define_components_models(ecs* world) {
    zoxd_tag(Model);
    zoxd_tag(ModelGroup);
    zoxd_tag(ModelCharacter);
    zoxd_byte(EyeRidge);
    zoxd_byte3(EyeSize);
    zoxd_byte(PlaceType);
    zoxd_byte(NodeVoxel);
    zoxd_byte(NodeColors);
}
