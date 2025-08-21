#include "hierarchy_refresh_system.c"
#include "inspector_element_system.c"

void zox_define_systems_editor(ecs_world_t *world) {
    zox_system(InspectorElementSystem, EcsOnUpdate,
        [in] generic.EntityTarget,
        [in] generic.ComponentTarget,
        [none] InspectorLabel)
    zox_system_1(HierarchyRefreshSystem, zoxp_mainthread,
        [in] transforms2.Position2,
        [in] layouts2.CanvasPosition,
        [in] layouts2.Layer2D,
        [in] layouts2.Anchor,
        [in] elements2.ListUIMax,
        [in] elements.core.ElementFontSize,
        [in] layouts2.CanvasLink,
        [out] HierarchyUIDirty,
        [out] layouts2.PixelPosition,
        [out] layouts2.PixelSize,
        [out] rendering.TextureSize,
        [out] hierarchys.Children,
        [none] HierarchyUI)
}