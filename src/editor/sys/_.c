#include "hierarchy2.c"
#include "inspector2.c"
#include "input.c"

void define_systems_editor(ecs *world) {

    /*zox_system(
        InspectorElementSystem,
        EcsOnUpdate,
        [in] core.EntityTarget,
        [in] core.ComponentTarget,
        [none] InspectorLabel
    );*/

    zox_system_1(
        HierarchySpawnSystem,
        zoxp_mainthread,
        [in] editor.HierarchyUIDirty,
        [in] layouts2.CanvasLink,
        [in] editor.EditorTarget,
        [in] elements2.ScrollviewLink,
        [in] elements.ElementFontSize
    );

    zox_system_1(
        InspectorSpawnSystem,
        zoxp_mainthread,
        [in] editor.InspectorDirty,
        [in] layouts2.CanvasLink,
        [in] editor.EditorTarget,
        [in] elements2.ScrollviewLink,
        [in] elements.ElementFontSize
    );

    /*zox_system_1(
        HierarchyRefreshSystem,
        zoxp_mainthread,
        [in] transforms2.Position2,
        [in] layouts2.CanvasPosition,
        [in] layouts2.Layer2D,
        [in] layouts2.Anchor,
        [in] layouts2.ListUIMax,
        [in] elements.ElementFontSize,
        [in] layouts2.CanvasLink,
        [in] editor.EditorTarget,
        [out] HierarchyUIDirty,
        [out] layouts2.LayoutPosition,
        [out] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [out] hierarchys.Children,
        [none] HierarchyUI
    );*/

    zox_system_1(
        EditorInputSystem,
        EcsPreStore,
        [in] inputs.DeviceLinks,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
}
