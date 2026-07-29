#if !defined(zoxm_transforms) && defined(zoxm_maths)
#define zoxm_transforms

#include "com/_.c"
#include "hierarchy/_.c"
#include "transforms2/_.c"
#include "transforms3/_.c"
#include "layouts2/_.c"

zox_begin_module(Transforms) {
    zoxd_components_transforms(world);
    zox_import_module(Hierarchys);
    zox_import_module(Transforms2);
    zox_import_module(Transforms3);
    zox_import_module(Layouts);
} zox_end_module(Transforms);

#endif
