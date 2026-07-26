#if !defined(zoxm_transforms) && defined(zoxm_maths)
#define zoxm_transforms

zox_tag(DisableTransform);
zox_tag(StaticTransform);
zoxc_float(Scale1);
zoxc_float(LocalScale1);
zoxc(TransformMatrix, float4x4);
#include "hierarchy/_.c"
#include "transforms2/_.c"
#include "transforms3/_.c"
#include "layouts2/_.c"

zox_begin_module(Transforms) {
    zoxd_tag(DisableTransform);
    zoxd_tag(StaticTransform);
    zoxd_float(Scale1);
    zoxd_float(LocalScale1);
    zoxd(TransformMatrix);
    zox_import_module(Hierarchys);
    zox_import_module(Transforms2);
    zox_import_module(Transforms3);
    zox_import_module(Layouts2);
} zox_end_module(Transforms);

#endif
