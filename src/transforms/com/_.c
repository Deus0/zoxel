zox_tag(DisableTransform);
zox_tag(DisableParentTransform);
zox_tag(DisableParentScale);
zox_tag(StaticTransform);
zoxc_float(Scale1);
zoxc_float(LocalScale1);
zoxc(TransformMatrix, float4x4);

void zoxd_components_transforms(ecs* world) {
    zoxd_tag(DisableTransform);
    zoxd_tag(DisableParentTransform);
    zoxd_tag(DisableParentScale);
    zoxd_tag(StaticTransform);
    zoxd_float(Scale1);
    zoxd_float(LocalScale1);
    zoxd(TransformMatrix);
}