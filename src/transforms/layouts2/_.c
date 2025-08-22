// Anchor/Grid Aware Positioning for our UIs and 2D Games!
#if !defined(zoxm_layouts2) && defined(zoxm_transforms2)
#define zoxm_layouts2

// TODO: Implement BoundToCanvas - Constraints in position systems itself, instead of functions

// TODO: Rename PixelPosition -> just LayoutPosition
// TODO: Rename PixelSize -> just LayoutSize
// TODO: Rename CanvasPosition -> just LayoutPosition
// TODO: Rename DragLimits -> just LayoutLimits2
// TODO: Set TextureSize when LayoutSizeDirty - If TextureSize is set to Match the LayoutSize
// TODO: also set MeshVertices2D when resizes
// TODO: just use LayoutParentData instead of [ParentSpawnData, CanvasSpawndata]
// TODO: WindowToTop -> just trigger Canvas's ChildrenDirty flag instead

#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "ins/_.c"

zox_begin_module(Layouts2)
    define_components_layouts2(world);
    define_systems_layouts2(world);
    add_hook_spawn_prefabs(spawn_prefabs_layouts2);
zox_end_module(Layouts2)

#endif