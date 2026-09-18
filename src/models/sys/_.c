#include "colors.c"
#include "fill.c"
#include "paint.c"
#include "merge.c"
#include "size.c"
#include "end.c"
#include "settings.c"

void zox_systems_models(ecs* world) {
    zox_system(
        ModelSizeNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] core.Seed,
        [out] models.ModelSize,
        [out] nodes.NodeEnd
    );
    zox_system(
        FillModelNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [in] models.ModelSize,
        [out] nodes.NodeEnd
    );
    zox_system(
        PaintModelNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [in] models.ModelSize,
        [out] nodes.NodeEnd
    );
    zox_system(
        ModelEndNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system(
        ColorsModelNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] core.Seed,
        [out] nodes.NodeEnd
    );
    zox_system_1(
        ModelsSettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings,
    );
    zox_system(
        ModelsSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
