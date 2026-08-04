#include "colors.c"
#include "fill.c"
#include "paint.c"
#include "merge.c"
#include "size.c"
#include "end.c"
#include "settings.c"
realm_clear_system(ModelLinks);

void define_systems_models(ecs* world) {
    realm_clear_systemd(rendering, ModelLinks);
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
        zoxp_mainthread,
        [in] settings.LoadSettings,
    );
    zox_system_1(
        ModelsSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
