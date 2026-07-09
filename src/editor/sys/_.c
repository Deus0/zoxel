#include "hierarchy.c"
#include "inspector.c"
#include "input.c"
#include "inspector_label.c"
#include "max_system_time.c"

void define_systems_editor(ecs *world) {
    zox_system_1(
        HierarchySpawnSystem,
        zoxp_mainthread,
        [in] editor.HierarchyUIDirty,
        [in] core.EntityTarget,
        [in] elements2.ScrollviewLink,
        [in] elements.ElementFontSize
    );
    zox_system_1(
        InspectorSpawnSystem,
        zoxp_mainthread,
        [in] editor.InspectorDirty,
        [in] core.EntityTarget,
        [in] elements2.ScrollviewLink,
        [in] elements.FontSize
    );
    zox_system_1(
        EditorInputSystem,
        zoxp_mainthread,
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
    zox_system(
        MaxSystemTimeLabelSystem,
        EcsOnUpdate,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] editor.MaxSystemTimeLabel
    );
}
