#include "hierarchy.c"
#include "inspector.c"
#include "input.c"
#include "inspector_label.c"
#include "max_system_time.c"
#include "fps.c"
#include "label.c"

void define_systems_editor(ecs *world) {
    zox_system(
        FpsDisplaySystem,
        zoxp_update,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [out] editor.FPSDisplayTicker,
        [none] editor.FPSDisplay
    );
    zox_system(
        DebugLabelSystem,
        zoxp_update,
        [in] editor.DebugLabelData,
        [out] texts.TextDirty,
        [out] texts.TextData,
        [none] editor.GameDebugLabel
    );
    zox_system(
        InspectorLabelSystem,
        zoxp_update,
        [in] rendering.RenderDisabled,
        [in] core.ComponentType,
        [in] core.EntityTarget,
        [in] core.ComponentTarget,
        [none] editor.InspectorLabel
    );
    zox_system_1(
        HierarchySpawnSystem,
        zoxp_spawn,
        [in] editor.HierarchyUIDirty,
        [in] core.EntityTarget,
        [in] elements2.ScrollviewLink,
        [in] ui.ElementFontSize
    );
    zox_system_1(
        InspectorSpawnSystem,
        zoxp_spawn,
        [in] editor.InspectorDirty,
        [in] core.EntityTarget,
        [in] elements2.ScrollviewLink,
        [in] ui.FontSize
    );
    zox_system_1(
        EditorInputSystem,
        zoxp_spawn,
        [none] players.Player
    );
#ifdef zox_time_systems
    zox_system(
        MaxSystemTimeLabelSystem,
        zoxp_update,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] editor.MaxSystemTimeLabel
    );
#endif
}
