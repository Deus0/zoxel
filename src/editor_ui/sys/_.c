#include "hierarchy_refresh_system.c"
#include "inspector_element_system.c"

void zox_define_systems_editor(ecs *world) {
    zox_system(InspectorElementSystem, EcsOnUpdate,
        [in] generic.EntityTarget,
        [in] generic.ComponentTarget,
        [none] InspectorLabel)
    zox_system_1(HierarchyRefreshSystem, zoxp_mainthread,
        [in] transforms2.Position2,
        [in] layouts2.CanvasPosition,
        [in] layouts2.Layer2D,
        [in] layouts2.Anchor,
        [in] layouts2.ListUIMax,
        [in] elements.ElementFontSize,
        [in] layouts2.CanvasLink,
        [out] HierarchyUIDirty,
        [out] layouts2.LayoutPosition,
        [out] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [out] hierarchys.Children,
        [none] HierarchyUI)
}