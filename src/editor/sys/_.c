#include "hierarchy.c"
#include "inspector.c"
#include "input.c"
#include "inspector_label.c"

void define_systems_editor(ecs *world) {
    zox_system_1(
        HierarchySpawnSystem,
        zoxp_mainthread,
        [in] editor.HierarchyUIDirty,
        [in] layouts2.CanvasLink,
        [in] core.EntityTarget,
        [in] elements2.ScrollviewLink,
        [in] elements.ElementFontSize
    );
    zox_system_1(
        InspectorSpawnSystem,
        zoxp_mainthread,
        [in] editor.InspectorDirty,
        [in] layouts2.CanvasLink,
        [in] core.EntityTarget,
        [in] elements2.ScrollviewLink,
        [in] elements.FontSize
    );
    zox_system_1(
        EditorInputSystem,
        EcsPreStore,
        [in] inputs.DeviceLinks,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
    zox_system(
        InspectorLabelSystem,
        EcsOnUpdate,
        [in] rendering.RenderDisabled,
        [in] core.ComponentType,
        [in] core.EntityTarget,
        [in] core.ComponentTarget,
        [none] editor.InspectorLabel
    );
}
