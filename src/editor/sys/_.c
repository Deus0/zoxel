#include "hierarchy.c"
#include "inspector.c"
#include "input.c"
#include "inspector_label.c"
#include "max_system_time.c"
#include "fps.c"
#include "label.c"

void zox_systems_editor(ecs *world) {
    zox_system(
        fps_display_system,
        zoxp_update,
        [out] texts.TextData,
        [out] editor.FPSDisplayTicker,
        [none] texts.Text,
        [none] editor.FPSDisplay,
    );
    zox_system(
        debug_label_system,
        zoxp_update,
        [in] editor.DebugLabelData,
        [out] texts.TextData,
        [none] texts.Text,
        [none] editor.GameDebugLabel
    );
    zox_system(
        inspector_label_system,
        zoxp_update,
        [in] rendering.RenderDisabled,
        [in] core.ComponentType,
        [in] core.Target,
        [in] core.ComponentTarget,
        [none] editor.InspectorLabel
    );
    zox_on_add(
        hierarchy_spawn_event,
        [in] core.Target,
        [in] ui.ElementFontSize,
        [none] editor.HierarchyUIDirty
    );
    zox_on_add(
        inspector_spawn_event,
        [in] core.Target,
        [in] ui.FontSize,
        [none] editor.InspectorDirty
    );
    zox_system_1(
        editor_input_system,
        zoxp_spawn,
        [none] players.Player
    );
#ifdef zox_time_systems
    zox_system(
        max_system_time_label_system,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] editor.MaxSystemTimeLabel
    );
#endif
}
